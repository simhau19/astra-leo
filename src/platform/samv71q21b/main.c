#include "app.h"
#include "readout.h"

#include "app_config.h"

#include "ethernet_phy.h"
#include "hal_ext_irq.h"
#include "hal_gpio.h"
#include "hal_mac_async.h"
#include "initialization.h"
#include "pins.h"

#include "param/param_server.h"
#include <FreeRTOSConfig.h>
#include <vmem/vmem.h>
#include <vmem/vmem_server.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "csp/csp.h"
#include "csp_eth_sam.h"
#include <csp/interfaces/csp_if_eth.h>

#include <stdint.h>
#include <stdio.h>

#define STACK_SIZE_VMEM_SERVER 256
#define STACK_SIZE_HEARTBEAT   configMINIMAL_STACK_SIZE

extern struct ethernet_phy_descriptor MACIF_PHY_desc;
extern struct mac_async_descriptor    MACIF;

static void heartbeat(void* param) {
    xTraceEnable(TRACE_START_OPTION);
    (void)param;
    for (;;) {
        gpio_toggle_pin_level(LED0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

extern QueueHandle_t readout_control_queue;

// the sw0 button simulates the interrupt from the readout fpga to signal data ready.
void sw0_cb(void) {

    if (readout_control_queue == NULL)
        return;

    ReadoutCtrlMsg msg;
    msg.type = READOUT_MSG_TYPE_DATA_READY;

    BaseType_t should_context_switch = pdFALSE;
    xQueueSendFromISR(readout_control_queue, &msg, &should_context_switch);

    // in case a higher priority task was unblocked because of the task notification, we should yield.
    portYIELD_FROM_ISR(should_context_switch);
}

int main(void) {

    int32_t ret;
    ret = board_init();
    while (ret)
        ;

    xTraceInitialize();

    static StaticTask_t heartbeat_tcb;
    static StackType_t  heartbeat_stack[STACK_SIZE_HEARTBEAT];
    xTaskCreateStatic(
        heartbeat,
        "Heartbeat",
        STACK_SIZE_HEARTBEAT,
        NULL,
        tskIDLE_PRIORITY + 1,
        heartbeat_stack,
        &heartbeat_tcb);

    /* Wait for PHY link up */
    bool link_up;

    do {
        ret = ethernet_phy_get_link_status(&MACIF_PHY_desc, &link_up);
        while (ret)
            ;
    } while (!link_up);

    csp_init();

    csp_iface_t* eth_iface;
    const char*  csp_eth_if_name = "eth";
    int          eth_mtu         = 1500;
    uint16_t     eth_if_addr     = CSP_ADDR_ASTRALEO;
    int          status;

    status = csp_eth_init(&MACIF, csp_eth_if_name, eth_mtu, eth_if_addr, true, &eth_iface);
    if (status != CSP_ERR_NONE) {
        printf("csp_eth_init returned with error code\r\n");
    }
    eth_iface->is_default = true;

    csp_bind_callback(csp_service_handler, CSP_ANY);
    csp_bind_callback(param_serve, PARAM_PORT_SERVER);

    // Even if not using vmem parameters,
    // this task is needed for other nodes to download the parameter list from ASTRA-leo
    static StaticTask_t vmem_server_tcb;
    static StackType_t  vmem_server_stack[STACK_SIZE_VMEM_SERVER];
    xTaskCreateStatic(
        vmem_server_loop,
        "vmem_server",
        STACK_SIZE_VMEM_SERVER,
        NULL,
        tskIDLE_PRIORITY,
        vmem_server_stack,
        &vmem_server_tcb);

    // wait for phy to settle. without this, mac_async_write calls might fail undetected
    volatile uint32_t i;
    for (i = 0; i < 1000000; i++)
        ;

    ext_irq_register(SW0, sw0_cb);

    app_start();

    while (1)
        ;
}
