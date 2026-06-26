
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "hpl_mac_async.h"
#include "portmacro.h"
#include "task.h"

#include <csp/csp.h>
#include <csp/csp_id.h>
#include <csp/csp_interface.h>
#include <csp/interfaces/csp_if_eth.h>
#include <csp/interfaces/csp_if_eth_pbuf.h>

#include "hal_mac_async.h"

#include <endian.h>
#include <limits.h>
#include <stdint.h>

#define STACK_SIZE_CSP_ETH_RX_LOOP 256

typedef struct {
    char                         name[CSP_IFLIST_NAME_MAX + 1];
    csp_eth_interface_data_t     ifdata;
    struct mac_async_descriptor* gmac_handle;
    // struct ifreq                 if_idx;
} eth_context_t;

static eth_context_t eth_ctx;

// TODO: make sure this buffer is protected such that it will not be modified when sending.
// Make sure it is not used before the gmac tx callback.
// alternatively copy into a gmac fifo / ringbuffer.
static uint8_t csp_eth_tx_buffer[CSP_ETH_BUF_SIZE];

static TaskHandle_t csp_eth_rx_loop_handle;

int csp_eth_tx_frame(void* driver_data, csp_eth_header_t* eth_frame) {

    const eth_context_t* ctx = (eth_context_t*)driver_data;

    size_t frame_size = sizeof(csp_eth_header_t) + be16toh(eth_frame->seg_size);

    if (mac_async_write(ctx->gmac_handle, (uint8_t*)eth_frame, frame_size) != ERR_NONE) {
        return CSP_ERR_DRIVER;
    }

    return CSP_ERR_NONE;
}

static void csp_eth_rx_loop(void* param) {
    eth_context_t* ctx = (eth_context_t*)param;

    static uint8_t    recvbuf[CSP_ETH_BUF_SIZE];
    csp_eth_header_t* eth_frame = (csp_eth_header_t*)recvbuf;

    while (1) {
        // counting semaphore behaviour.
        // If xTaskNotifyTake is called multiple times consecutively, this will unblock the same amount of times.
        //(standard producer consumer type stuff)
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        uint32_t received_len = mac_async_read(ctx->gmac_handle, recvbuf, CSP_ETH_BUF_SIZE);
        csp_eth_rx(&ctx->ifdata.iface, eth_frame, received_len, NULL);
    }
}

static void eth_rx_cb(void) {

    BaseType_t should_context_switch = pdFALSE;

    vTaskNotifyGiveFromISR(csp_eth_rx_loop_handle, &should_context_switch);

    // in case a higher priority task was unblocked because of the task notification, we should yield.
    portYIELD_FROM_ISR(should_context_switch);
}

static void eth_tx_cb(void) {
    // printf("tx\r\n");
}

int csp_eth_init(struct mac_async_descriptor* gmac_handle, const char* ifname, uint16_t mtu, uint16_t node_id, bool promisc, csp_iface_t** return_iface) {

    uint8_t mac_addr[CSP_ETH_ALEN] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};

    eth_context_t* ctx = &eth_ctx;

    memset(ctx, 0, sizeof(*ctx));

    strncpy(ctx->name, ifname, sizeof(ctx->name) - 1);
    ctx->ifdata.iface.name           = ctx->name;
    ctx->ifdata.tx_func              = &csp_eth_tx_frame;
    ctx->ifdata.tx_buf               = (csp_eth_header_t*)&csp_eth_tx_buffer;
    ctx->ifdata.iface.nexthop        = &csp_eth_tx;
    ctx->ifdata.iface.addr           = node_id;
    ctx->ifdata.iface.driver_data    = ctx;
    ctx->ifdata.iface.interface_data = &ctx->ifdata;
    ctx->ifdata.promisc              = promisc;
    ctx->ifdata.tx_mtu               = mtu;

    ctx->gmac_handle = gmac_handle;

    memcpy(ctx->ifdata.if_mac, mac_addr, CSP_ETH_ALEN);

    /* Ether header 14 byte, seg header 4 byte, CSP header 6 byte */
    if (mtu < 24) {
        csp_print("csp_if_eth_init: mtu < 24\n");
        return CSP_ERR_INVAL;
    }

    // filter
    struct mac_async_filter mac_filter;
    memcpy(mac_filter.mac, mac_addr, CSP_ETH_ALEN);
    mac_filter.tid[0]     = (CSP_ETH_TYPE_CSP >> 8);
    mac_filter.tid[1]     = (CSP_ETH_TYPE_CSP & 0xFF);
    mac_filter.tid_enable = true; // Only allow CSP Packets
                                  //
    volatile int32_t status = mac_async_set_filter(gmac_handle, 1, &mac_filter);

    status = mac_async_register_callback(gmac_handle, MAC_ASYNC_RECEIVE_CB, eth_rx_cb);
    status = mac_async_register_callback(gmac_handle, MAC_ASYNC_TRANSMIT_CB, eth_tx_cb);

    static StaticTask_t csp_eth_rx_loop_tcb;
    static StackType_t  csp_eth_rx_loop_stack[STACK_SIZE_CSP_ETH_RX_LOOP];
    csp_eth_rx_loop_handle = xTaskCreateStatic(
        csp_eth_rx_loop,
        "CSP Rx loop",
        STACK_SIZE_CSP_ETH_RX_LOOP,
        ctx,
        tskIDLE_PRIORITY + 4,
        csp_eth_rx_loop_stack,
        &csp_eth_rx_loop_tcb);

    csp_iflist_add(&ctx->ifdata.iface);

    if (return_iface) {
        *return_iface = &ctx->ifdata.iface;
    }

    // mac_async_init
    // mac_async_set_filter
    // mac_async_enable
    // mac_async_enable_irq
    // mac_async_register_callback

    return CSP_ERR_NONE;
}
