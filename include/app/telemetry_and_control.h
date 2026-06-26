#ifndef TELEMETRY_AND_CONTROL_H
#define TELEMETRY_AND_CONTROL_H

#include "data_handler.h"
#include "param/param.h"

#define NUM_MODES 9

typedef enum {
    OPMODE_OFF         = 0,
    OPMODE_INIT        = 1,
    OPMODE_STANDBY     = 2,
    OPMODE_COUNTING    = 3,
    OPMODE_SCIENCE     = 4,
    OPMODE_ADVANCED    = 5,
    OPMODE_SYNC        = 6,
    OPMODE_CALIBRATION = 7,
    OPMODE_DEBUG       = 8,
} OpMode;

typedef param_t* ParamsWriteMsg;

typedef struct {
    QueueHandle_t params_write_queue;
    QueueHandle_t readout_control_queue;
    QueueHandle_t readout_response_queue;
} TelemetryAndControlArgs;

int set_op_mode(OpMode mode);

void telemetry_and_control_task(void* param);

#endif // TELEMETRY_AND_CONTROL_H
