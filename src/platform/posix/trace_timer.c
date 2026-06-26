#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>

static uint32_t ulEntryTime = 0;

void vTraceTimerReset(void){
    ulEntryTime = (uint32_t)xTaskGetTickCount();
}

uint32_t uiTraceTimerGetFrequency(void){
    return configTICK_RATE_HZ;
}

uint32_t uiTraceTimerGetValue(void){
    return ((uint32_t)xTaskGetTickCount() - ulEntryTime);
}
