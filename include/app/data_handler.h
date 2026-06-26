#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include "siphra.h"

#include <FreeRTOS.h>
#include <queue.h>

#include <stdint.h>

typedef struct {
    uint16_t      server_addr;
    uint8_t       server_port;
    QueueHandle_t input_queue;
} DataHandlerArgs;

typedef enum {
    DATA_HANDLER_MESSAGE_TYPE_SPECTROSCOPY,
    DATA_HANDLER_MESSAGE_TYPE_COUNT,
} DataHandlerMsgType;

typedef struct {
    uint8_t            temp_probes[16];
    DataHandlerMsgType type;
    union {
        SiphraSpectroscopy   spectroscopy_data;
        SiphraDetectionCount count;
    };
} DataHandlerMsg;

void data_handler_task(void* param);

#endif // DATA_HANDLER_H
