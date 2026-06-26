#include "app.h"

#include "aldp.h"
#include "app_config.h"
#include "data_handler.h"
#include "readout.h"
#include "sys_error.h"
#include "telemetry_and_control.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "csp/csp.h"

#include <stdio.h>

ReadoutArgs             readout_args;
DataHandlerArgs         data_handler_args;
TelemetryAndControlArgs telemetry_and_control_args;

QueueHandle_t readout_data_queue             = NULL;
QueueHandle_t readout_control_response_queue = NULL;
QueueHandle_t readout_control_queue          = NULL;
QueueHandle_t params_write_queue             = NULL;

static void router(void* param) {
    (void)param;

    csp_print("Routing task started\r\n");
    while (1) {
        csp_route_work();
    }
}

void app_start(void) {

    static StaticQueue_t readout_data_qb, readout_control_response_qb, readout_control_qb, params_write_qb;
    static uint8_t       readout_data_queue_buffer[10 * sizeof(DataHandlerMsg)];
    static uint8_t       readout_control_response_queue_buffer[10 * sizeof(SysError)];
    static uint8_t       readout_control_queue_buffer[10 * sizeof(ReadoutCtrlMsg)];
    static uint8_t       params_write_queue_buffer[10 * sizeof(ParamsWriteMsg)];

    readout_data_queue = xQueueCreateStatic(
        10,
        sizeof(DataHandlerMsg),
        readout_data_queue_buffer,
        &readout_data_qb);

    readout_control_response_queue = xQueueCreateStatic(
        10,
        sizeof(SysError),
        readout_control_response_queue_buffer,
        &readout_control_response_qb);

    readout_control_queue = xQueueCreateStatic(
        10,
        sizeof(ReadoutCtrlMsg),
        readout_control_queue_buffer,
        &readout_control_qb);

    params_write_queue = xQueueCreateStatic(
        10,
        sizeof(ParamsWriteMsg),
        params_write_queue_buffer,
        &params_write_qb);

    static StaticTask_t csp_router_tcb;
    static StackType_t  csp_router_stack[STACK_SIZE_CSP_ROUTER];
    xTaskCreateStatic(router,
                      "CSP Router",
                      STACK_SIZE_CSP_ROUTER,
                      NULL,
                      TASK_PRIORITY_CSP_ROUTER,
                      csp_router_stack,
                      &csp_router_tcb);

    readout_args.input_queue            = readout_control_queue;
    readout_args.data_output_queue      = readout_data_queue;
    readout_args.control_response_queue = readout_control_response_queue;

    static StaticTask_t readout_tcb;
    static StackType_t  readout_stack[STACK_SIZE_READOUT];
    xTaskCreateStatic(readout_task,
                      "Readout",
                      STACK_SIZE_READOUT,
                      &readout_args,
                      TASK_PRIORITY_READOUT,
                      readout_stack,
                      &readout_tcb);

    data_handler_args.input_queue = readout_data_queue;
    data_handler_args.server_addr = CSP_ADDR_SDR0;
    data_handler_args.server_port = CSP_PORT_ALDP;

    static StaticTask_t data_handler_tcb;
    static StackType_t  data_handler_stack[STACK_SIZE_DATA_HANDLER];
    xTaskCreateStatic(data_handler_task,
                      "Data handler",
                      STACK_SIZE_DATA_HANDLER,
                      &data_handler_args,
                      TASK_PRIORITY_DATA_HANDLER,
                      data_handler_stack,
                      &data_handler_tcb);

    telemetry_and_control_args.params_write_queue     = params_write_queue;
    telemetry_and_control_args.readout_control_queue  = readout_control_queue;
    telemetry_and_control_args.readout_response_queue = readout_control_response_queue;

    static StaticTask_t telemetry_and_control_tcb;
    static StackType_t  telemetry_and_control_stack[STACK_SIZE_DATA_HANDLER];
    xTaskCreateStatic(telemetry_and_control_task,
                      "Telem & Ctrl",
                      STACK_SIZE_TELEMETRY_AND_CONTROL,
                      &telemetry_and_control_args,
                      TASK_PRIORITY_TELEMETRY_AND_CONTROL,
                      telemetry_and_control_stack,
                      &telemetry_and_control_tcb);

    // csp_print("Connection table\r\n");
    // csp_conn_print_table();

    // csp_print("Interfaces\r\n");
    // csp_iflist_print();

    vTaskStartScheduler();
}
