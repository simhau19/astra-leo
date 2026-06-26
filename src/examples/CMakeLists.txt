add_library(
    app_server_client_example
    STATIC
    server_client_example.c
    ../app/hooks.c
)

target_include_directories(
    app_server_client_example
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/app
        ${CMAKE_SOURCE_DIR}/include/interface
        ${CMAKE_SOURCE_DIR}/include/platform
)

target_link_libraries(
    app_server_client_example
    PRIVATE csp freertos_kernel compiler_warnings
)
