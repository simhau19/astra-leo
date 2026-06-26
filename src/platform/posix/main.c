#include "app.h"

#include "FreeRTOS.h"
#include "app_config.h"
#include "task.h"

#include "csp/csp.h"
#include "csp/drivers/eth_linux.h"

#include "param/param_server.h"
#include <vmem/vmem.h>
#include <vmem/vmem_server.h>

#include <stddef.h> // Defines NULL
#include <stdio.h>
#include <stdlib.h> // Defines EXIT_FAILURE

#define STACK_SIZE_VMEM_SERVER 256

int main(int argc, char* argv[]) {

    (void)argc;
    (void)argv;

    xTraceInitialize();
    xTraceEnable(TRACE_START_OPTION);

    csp_init();

    csp_iface_t* eth_iface;
    const char*  eth_device      = "veth0";
    const char*  csp_eth_if_name = "veth";
    int          eth_mtu         = 1500;
    uint16_t     eth_if_addr     = CSP_ADDR_ASTRALEO;

    int status;
    status = csp_eth_init(eth_device, csp_eth_if_name, eth_mtu, eth_if_addr, true, &eth_iface);
    if (status != CSP_ERR_NONE) {
        printf("csp_eth_init returned with error code %d. terminating\r\n", status);
        return 1;
    }
    eth_iface->is_default = true;

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

    app_start();

    /* Execution should not come here during normal operation */
    return (EXIT_FAILURE);
}

/*******************************************************************************
 End of File
*/
