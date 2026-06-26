#include "app.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "csp/csp_types.h"
#include <csp/csp.h>
#include <csp/csp_debug.h>

#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>

#define SERVER_PORT 10
#define SERVER_ADDR 0

#define STACK_SIZE_SERVER 256
#define STACK_SIZE_CLIENT 256
#define STACK_SIZE_CSP_ROUTER 256

static uint32_t server_received = 0;

static void server(void* param) {
    (void)param;

    csp_print("Server task started \r\n");

    csp_socket_t sock = {0};
    csp_bind(&sock, CSP_ANY);
    csp_listen(&sock, 10);

    while (1) {
        csp_conn_t* conn;

        if ((conn = csp_accept(&sock, 10000)) == NULL) {
            continue;
        }

        csp_packet_t* packet;
        while ((packet = csp_read(conn, 50)) != NULL) {
            switch (csp_conn_dport(conn)) {
            case SERVER_PORT:
                csp_print("Packet received: %s\r\n", (char*)packet->data);
                csp_buffer_free(packet);
                server_received++;
                break;
            default:
                csp_service_handler(packet);
                break;
            }
        }
        csp_close(conn);
    }
}

static void client(void* param) {
    (void)param;

    csp_print("Client task started\r\n");

    uint32_t count = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        int result = csp_ping(SERVER_ADDR, 1000, 100, CSP_O_NONE);
        csp_print("Ping address: %u, result: %d [ms]\r\n", SERVER_ADDR, result);

        csp_reboot(SERVER_ADDR);
        csp_print("reboot system request sent to addr: %u\r\n", SERVER_ADDR);

        csp_conn_t* conn = csp_connect(
            CSP_PRIO_NORM, SERVER_ADDR, SERVER_PORT, 1000, CSP_O_NONE);
        if (conn == NULL) {
            csp_print("Connection failed :(\r\n");
            vTaskDelete(NULL); // self destruct
        }

        csp_packet_t* packet = csp_buffer_get(0);
        if (packet == NULL) {
            csp_print("Failed to get buffer\r\n");
            csp_close(conn);
        }

        memcpy(packet->data, "Hello World!", 12);
        sprintf((char*)&packet->data, "Hello World! %u\r\n", count++);
        packet->length = (uint16_t)strlen((char*)packet->data) + 1;

        csp_send(conn, packet);

        csp_close(conn);
    }
}

static void router(void* param) {
    (void)param;

    csp_print("Routing task started\r\n");
    while (1) {
        csp_route_work();
    }
}

void app_start() {
    csp_print("Initialising CSP\r\n");
    csp_init();

    static StaticTask_t csp_router_tcb;
    static StackType_t csp_router_stack[STACK_SIZE_CSP_ROUTER];
    xTaskCreateStatic(router,
                      "CSP Router",
                      STACK_SIZE_CSP_ROUTER,
                      NULL,
                      tskIDLE_PRIORITY + 1,
                      csp_router_stack,
                      &csp_router_tcb);

    csp_print("Connection table \r\n") csp_conn_print_table();

    csp_print("Interfaces\r\n");
    csp_iflist_print();

    static StaticTask_t server_tcb;
    static StackType_t server_stack[STACK_SIZE_SERVER];
    xTaskCreateStatic(server,
                      "Server",
                      STACK_SIZE_SERVER,
                      NULL,
                      tskIDLE_PRIORITY + 2,
                      server_stack,
                      &server_tcb);

    static StaticTask_t client_tcb;
    static StackType_t client_stack[STACK_SIZE_CLIENT];
    xTaskCreateStatic(client,
                "Client",
                STACK_SIZE_CLIENT,
                NULL,
                tskIDLE_PRIORITY + 1,
                client_stack,
                &client_tcb);

    vTaskStartScheduler();

    // Sould never reach this point unless explicitly ending the scheduler.
    // If it still reaches this point it is likely because there was an error
    // allocating memory for the idle or timer task stack.
    while (true)
        ;
}
