#include "telemetry_and_control.h"

#include <param/param.h>
#include <param_config.h>

#include <FreeRTOS.h>
#include <queue.h>

extern QueueHandle_t params_write_queue;

static void param_write_cb(const param_t* param, int index) {
    (void)index;

    xQueueSend(params_write_queue, &param, portMAX_DELAY);
}

OpMode _current_mode = OPMODE_OFF;
PARAM_DEFINE_STATIC_RAM(
    PARAMID_ALEO_CURRENT_MODE,
    aleo_current_mode,
    PARAM_TYPE_UINT8,
    0,
    0,
    PM_READONLY,
    NULL,
    NULL,
    (uint8_t*)&_current_mode,
    "ASTRA-LEO Current Mode of Operation");

OpMode _requested_mode = OPMODE_OFF;
PARAM_DEFINE_STATIC_RAM(
    PARAMID_ALEO_REQUESTED_MODE,
    aleo_requested_mode,
    PARAM_TYPE_UINT8,
    0,
    0,
    PM_CONF,
    param_write_cb,
    NULL,
    (uint8_t*)&_requested_mode,
    "ASTRA-LEO Requested Mode of Operation");
