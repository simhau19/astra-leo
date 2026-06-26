#include "telemetry_and_control.h"
#include "param_config.h"
#include "portmacro.h"
#include "readout.h"

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stdint.h>
#include <stdio.h>

extern param_t aleo_current_mode;

OpMode get_current_mode(void) {
    OpMode mode;
    param_get(&aleo_current_mode, 0, &mode);
    return mode;
}

bool is_mode_change_valid(OpMode from, OpMode to) {

    // This table represents the valid mode changes that can be requested via libparam
    // This can be optimized by compressing the table to 9 uint16_t (18B) instead of 9*9 bool (81B)
    static const bool mode_change_table[NUM_MODES][NUM_MODES] = {
        // FRM M2 M3 M4 M5 M6 M7 M8     TO:
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, // M0 (POWER_OFF)
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, // M1 (INIT)
        {0, 0, 1, 1, 1, 1, 1, 1, 1}, // M2 (STANDBY)
        {0, 0, 1, 1, 1, 1, 0, 0, 0}, // M3 (COUNTING)
        {0, 0, 1, 1, 1, 1, 0, 0, 0}, // M4 (SCIENCE)
        {0, 0, 1, 1, 1, 1, 0, 0, 0}, // M5 (ADVANCED)
        {0, 0, 1, 1, 1, 1, 1, 0, 0}, // M6 (SYNC)
        {0, 0, 1, 1, 1, 1, 0, 1, 0}, // M7 (CALIBRATION)
        {0, 0, 1, 1, 1, 1, 0, 0, 1}, // M8 (DEBUG)
    };

    uint16_t from_idx = (uint16_t)from;
    uint16_t to_idx   = (uint16_t)to;

    if (from_idx >= NUM_MODES || to_idx >= NUM_MODES) {
        // error. should never get here
        return 0;
    }

    return mode_change_table[from_idx][to_idx];
}

void try_change_opmode(OpMode new_mode) {
    if (is_mode_change_valid(get_current_mode(), new_mode)) {
        param_set(&aleo_current_mode, 0, &new_mode);
    } 
}

void telemetry_and_control_task(void* param) {

    TelemetryAndControlArgs* args = (TelemetryAndControlArgs*)param;

    ParamsWriteMsg param_to_set;

    printf("telemetry and control task started \r\n");

    for (;;) {

        xQueueReceive(args->params_write_queue, &param_to_set, portMAX_DELAY);

        if (param_to_set->id == PARAMID_ALEO_REQUESTED_MODE) {
            OpMode new_mode;
            param_get_data(param_to_set, &new_mode, sizeof(OpMode));
            if (new_mode != get_current_mode()) {
                try_change_opmode(new_mode);
                ReadoutCtrlMsg msg;
                msg.type = READOUT_MSG_TYPE_MODE_CHANGE;
                xQueueSend(args->readout_control_queue, &msg, pdMS_TO_TICKS(100));
                // SysError response;
                // xQueueReceive(args->readout_response_queue, &response, pdMS_TO_TICKS(100));
            }
        }
    }
}
