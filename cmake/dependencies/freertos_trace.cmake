FetchContent_Declare(
    freertos_trace
    GIT_REPOSITORY https://github.com/percepio/TraceRecorderSource.git
    GIT_TAG ${FREERTOS_TRACE_VERSION}
)

FetchContent_MakeAvailable(freertos_trace)

set(FREERTOS_TRACE_STREAM_PORT
    RINGBUFFER
    CACHE STRING
    "Stream Port for FreeRTOS Trace"
)

add_library(freertos_trace_include INTERFACE)

add_library(
    freertos_trace
    STATIC
    ${freertos_trace_SOURCE_DIR}/trcAssert.c
    ${freertos_trace_SOURCE_DIR}/trcCounter.c
    ${freertos_trace_SOURCE_DIR}/trcDependency.c
    ${freertos_trace_SOURCE_DIR}/trcDiagnostics.c
    ${freertos_trace_SOURCE_DIR}/trcEntryTable.c
    ${freertos_trace_SOURCE_DIR}/trcError.c
    ${freertos_trace_SOURCE_DIR}/trcEvent.c
    ${freertos_trace_SOURCE_DIR}/trcEventBuffer.c
    ${freertos_trace_SOURCE_DIR}/trcExtension.c
    ${freertos_trace_SOURCE_DIR}/trcHardwarePort.c
    ${freertos_trace_SOURCE_DIR}/trcHeap.c
    ${freertos_trace_SOURCE_DIR}/trcISR.c
    ${freertos_trace_SOURCE_DIR}/trcInternalEventBuffer.c
    ${freertos_trace_SOURCE_DIR}/trcInterval.c
    ${freertos_trace_SOURCE_DIR}/trcMultiCoreEventBuffer.c
    ${freertos_trace_SOURCE_DIR}/trcObject.c
    ${freertos_trace_SOURCE_DIR}/trcPrint.c
    ${freertos_trace_SOURCE_DIR}/trcRunnable.c
    ${freertos_trace_SOURCE_DIR}/trcStackMonitor.c
    ${freertos_trace_SOURCE_DIR}/trcStateMachine.c
    ${freertos_trace_SOURCE_DIR}/trcStaticBuffer.c
    ${freertos_trace_SOURCE_DIR}/trcStreamingRecorder.c
    ${freertos_trace_SOURCE_DIR}/trcString.c
    ${freertos_trace_SOURCE_DIR}/trcTask.c
    ${freertos_trace_SOURCE_DIR}/trcTaskMonitor.c
    ${freertos_trace_SOURCE_DIR}/trcTimestamp.c
    ${freertos_trace_SOURCE_DIR}/kernelports/FreeRTOS/trcKernelPort.c
)

target_include_directories(
    freertos_trace_include
    INTERFACE
        ${freertos_trace_SOURCE_DIR}/include
        ${freertos_trace_SOURCE_DIR}/kernelports/FreeRTOS/include
)

target_link_libraries(freertos_trace freertos_trace_include freertos_kernel)

target_link_libraries(freertos_config INTERFACE freertos_trace_include)
target_include_directories(
    freertos_config
    INTERFACE ${CMAKE_SOURCE_DIR}/include/trace_config
)

if(FREERTOS_TRACE_STREAM_PORT STREQUAL "RINGBUFFER")
    target_sources(
        freertos_trace
        PRIVATE
            ${freertos_trace_SOURCE_DIR}/streamports/RingBuffer/trcStreamPort.c
    )
    target_include_directories(
        freertos_trace_include
        INTERFACE ${freertos_trace_SOURCE_DIR}/streamports/RingBuffer/include
    )
    target_include_directories(
        freertos_config
        INTERFACE ${CMAKE_SOURCE_DIR}/include/trace_config/ringbuffer
    )
    target_compile_definitions(
        freertos_config
        INTERFACE TRACE_START_OPTION=TRC_START
    )
elseif(FREERTOS_TRACE_STREAM_PORT STREQUAL "RTT")
    target_sources(
        freertos_trace
        PRIVATE
            ${freertos_trace_SOURCE_DIR}/streamports/Jlink_RTT/trcStreamPort.c
            ${freertos_trace_SOURCE_DIR}/streamports/Jlink_RTT/SEGGER_RTT.c
    )
    target_include_directories(
        freertos_trace_include
        INTERFACE ${freertos_trace_SOURCE_DIR}/streamports/Jlink_RTT/include
    )
    target_include_directories(
        freertos_config
        INTERFACE ${CMAKE_SOURCE_DIR}/include/trace_config/rtt
    )
    target_compile_definitions(
        freertos_config
        INTERFACE TRACE_START_OPTION=TRC_START_FROM_HOST
    )
else()
    message(
        FATAL_ERROR
        "Invalid stream port. Add a new port by configuring it here"
    )
endif()
