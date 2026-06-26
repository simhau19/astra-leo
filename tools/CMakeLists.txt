add_executable(dummy_payload_controller dummy_payload_controller.c)
target_link_libraries(dummy_payload_controller PRIVATE)

target_sources(
    dummy_payload_controller
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/app/hooks.c
        ${CMAKE_SOURCE_DIR}/src/platform/posix/eth_linux.c
        ${CMAKE_SOURCE_DIR}/src/platform/posix/trace_timer.c
)

target_link_libraries(
    dummy_payload_controller
    PRIVATE compiler_warnings freertos_kernel freertos_config freertos_trace csp
)

target_include_directories(
    dummy_payload_controller
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include/interface
        ${CMAKE_SOURCE_DIR}/include/app
)
