#include "readout.h"
#include "siphra.h"

#include <stdint.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "portmacro.h"
#include "projdefs.h"

void readout_task(void* param) {
    printf("readout task started \r\n");
    ReadoutArgs* args = (ReadoutArgs*)param;

    ReadoutCtrlMsg msg;

    for (;;) {
        BaseType_t status;
        status = xQueueReceive(args->input_queue, &msg, portMAX_DELAY);
        if (status != pdPASS)
            continue;

        switch (msg.type) {
        case READOUT_MSG_TYPE_DATA_READY: {
            DataHandlerMsg data;

            siphra_do_readout(&data.spectroscopy_data);

            data.type = DATA_HANDLER_MESSAGE_TYPE_SPECTROSCOPY;

            // get temperature data
            data.temp_probes[0]  = 0;
            data.temp_probes[1]  = 1;
            data.temp_probes[2]  = 2;
            data.temp_probes[3]  = 3;
            data.temp_probes[4]  = 4;
            data.temp_probes[5]  = 5;
            data.temp_probes[6]  = 6;
            data.temp_probes[7]  = 7;
            data.temp_probes[8]  = 8;
            data.temp_probes[9]  = 9;
            data.temp_probes[10] = 10;
            data.temp_probes[11] = 11;
            data.temp_probes[12] = 12;
            data.temp_probes[13] = 13;
            data.temp_probes[14] = 14;
            data.temp_probes[15] = 15;

            // read temperature data
            xQueueSend(args->data_output_queue, &data, portMAX_DELAY);
        } break;
        case READOUT_MSG_TYPE_MODE_CHANGE: {

        } break;
        default:
            // should not end up here
            break;
        }
    }
}
