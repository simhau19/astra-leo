target_include_directories(freertos_config SYSTEM INTERFACE config)

# add_executable(demo)
# target_sources(
#     demo
#     PRIVATE
#         ${csp_SOURCE_DIR}/examples/csp_arch.c
#         ${CMAKE_SOURCE_DIR}/src/app/hooks.c
# )

# target_link_libraries(demo PRIVATE freertos_kernel freertos_config csp)

add_executable(
    server_client_example
    ${CMAKE_SOURCE_DIR}/src/platform/posix/main.c
    ./eth_linux.c
    ./trace_timer.c
    ${CMAKE_SOURCE_DIR}/src/app/hooks.c
)

target_link_libraries(
    server_client_example
    PRIVATE
        freertos_kernel
        freertos_config
        freertos_trace
        csp
        libparam
        params
        app_server_client_example
        compiler_warnings
)

add_executable(
    main
    ${CMAKE_SOURCE_DIR}/src/platform/posix/main.c
    ./eth_linux.c
    ./trace_timer.c
    ${CMAKE_SOURCE_DIR}/src/app/hooks.c
)

target_link_libraries(
    main
    freertos_kernel
    freertos_config
    freertos_trace
    libparam
    params
    csp
    app_main
    compiler_warnings
)
