#ifndef READOUT_H
#define READOUT_H

#include <telemetry_and_control.h>

#include <FreeRTOS.h>
#include <queue.h>

typedef enum {
    READOUT_MSG_TYPE_DATA_READY = 0,
    READOUT_MSG_TYPE_MODE_CHANGE,
} ReadoutMsgType;

typedef struct {
    ReadoutMsgType type;
} ReadoutCtrlMsg;

typedef struct {
    xQueueHandle data_output_queue;
    xQueueHandle control_response_queue;
    xQueueHandle input_queue;
} ReadoutArgs;

void readout_task(void* param);

#endif // READOUT_H
