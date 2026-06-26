#include "FreeRTOS.h"
#include "portmacro.h"
#include "task.h"
#include <FreeRTOSConfig.h>
#include <stdio.h>

void vApplicationIdleHook(void) {}
void vApplicationTickHook(void) {}
void vApplicationDaemonTaskStartupHook(void) {}

// Error handling hooks
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char* pcTaskName) {

    (void)pcTaskName;
    (void)pxTask;

    configASSERT(0);
}

void vApplicationMallocFailedHook(void) {
    configASSERT(0);
}

void vAssertCalled(const char* const pcFileName, unsigned long ulLine) {
    printf("ASSERTION in file %s:%lu\r\n", pcFileName, ulLine);
    //disable interrupts or stop scheduler
    while(1);
}

// Static allocation hooks
void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                   StackType_t**  ppxIdleTaskStackBuffer,
                                   uint32_t*      pulIdleTaskStackSize) {
    static StaticTask_t xIdleTaskTCB;
    static StackType_t  uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer   = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                                    StackType_t**  ppxTimerTaskStackBuffer,
                                    uint32_t*      pulTimerTaskStackSize) {

    static StaticTask_t xTimerTaskTCB;
    static StackType_t  uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer   = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
}
