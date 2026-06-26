cmake_minimum_required(VERSION 3.24)
project(astra_leo_fw C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/platforms/${PLATFORM}.cmake)
include(cmake/warnings.cmake)

#DEPENDENCIES
include(FetchContent)

set(FREERTOS_KERNEL_VERSION V11.2.0)
set(FREERTOS_TRACE_VERSION Tz4/4.11/v4.11.0)

set(CSP_VERSION 51628cd7a208edff81eff9b2b6fadc70dea5c5a4) #Newer than V2.1

#Same version that the Payload controller uses
# set(LIBPARAM_VERSION c1fcd4d26e8918d39371cff5ff44072ed7a2d8d2)
set(LIBPARAM_VERSION v2.0)

#FreeRTOS
FetchContent_Declare(
    freertos-kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG ${FREERTOS_KERNEL_VERSION}
)

add_library(freertos_config INTERFACE)

#CSP
FetchContent_Declare(
    csp
    GIT_REPOSITORY https://github.com/libcsp/libcsp.git
    GIT_TAG ${CSP_VERSION}
)

# tell CSP to build with FreeRTOS
set(CMAKE_SYSTEM_NAME Generic)
set(CSP_SYSTEM_NAME freertos)

FetchContent_MakeAvailable(freertos-kernel)
FetchContent_MakeAvailable(csp)
include(cmake/dependencies/libparam.cmake)
include(cmake/dependencies/freertos_trace.cmake)

add_subdirectory(src/app)
add_subdirectory(src/param)
add_subdirectory(src/examples)
add_subdirectory(src/platform)

if(PLATFORM STREQUAL "posix")
    add_subdirectory(tools)
endif()
