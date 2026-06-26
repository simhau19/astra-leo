add_library(
    app_main
    STATIC
    app_main.c
    hooks.c
    readout.c
    data_handler.c
    telemetry_and_control.c
)

target_include_directories(
    app_main
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/app
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/interface
        ${CMAKE_SOURCE_DIR}/include/platform
        ${CMAKE_SOURCE_DIR}/include/drivers
)

target_link_libraries(
    app_main
    PRIVATE csp libparam freertos_kernel compiler_warnings
)
