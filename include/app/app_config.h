#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "FreeRTOS.h"
#include "task.h"

#define CSP_ADDR_ASTRALEO_0 1025U
#define CSP_ADDR_ASTRALEO_1 1029U

#define CSP_ADDR_SDR0_0 1024U // for ASTRA-LEO 0 Subnet
#define CSP_ADDR_SDR0_1 1028U // for ASTRA-LEO 1 Subnet

#define CSP_PORT_ALDP 15U

// Set the correct ASTRA-LEO ID here
// CENSSAT-1 Has two ASTRA-LEOs, so the ID should be either 0 or 1
#define ASTRALEO_ID 0U

#ifndef ASTRALEO_ID
#error "ASTRALEO_ID not defined. HINT: Sould be defined as either 0 or 1 via a compile flag"
#elif ASTRALEO_ID == 0
#define CSP_ADDR_ASTRALEO CSP_ADDR_ASTRALEO_0
#define CSP_ADDR_SDR0     CSP_ADDR_SDR0_0
#elif ASTRALEO_ID == 1
#define CSP_ADDR_ASTRALEO CSP_ADDR_ASTRALEO_1
#define CSP_ADDR_SDR0     CSP_ADDR_SDR0_1
#else
#error "ASTRALEO_ID invalid"
#endif

#define STACK_SIZE_CSP_ROUTER            1024U
#define STACK_SIZE_DATA_HANDLER          1024U
#define STACK_SIZE_READOUT               1024U
#define STACK_SIZE_TELEMETRY_AND_CONTROL 1024U

#define TASK_PRIORITY_CSP_ROUTER            (tskIDLE_PRIORITY + 3U)
#define TASK_PRIORITY_DATA_HANDLER          (tskIDLE_PRIORITY + 3U)
#define TASK_PRIORITY_READOUT               (tskIDLE_PRIORITY + 3U)
#define TASK_PRIORITY_TELEMETRY_AND_CONTROL (tskIDLE_PRIORITY + 3U)

#endif // APP_CONFIG_H
