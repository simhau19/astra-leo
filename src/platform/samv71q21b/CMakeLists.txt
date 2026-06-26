add_library(
    asf4
    OBJECT
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/core/hpl_init.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/core/hpl_core_m7_base.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/gmac/hpl_gmac.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/pmc/hpl_pmc.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/pmc/hpl_sleep.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/pio/hpl_pio_ext.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hpl/usart/hpl_usart.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_init.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_mac_async.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_usart_sync.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_atomic.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_io.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/src/hal_ext_irq.c
    ${CMAKE_SOURCE_DIR}/src/asf4/hal/utils/src/utils_syscalls.c
    ${CMAKE_SOURCE_DIR}/src/asf4/stdio_redirect/stdio_io.c
    ${CMAKE_SOURCE_DIR}/src/asf4/stdio_redirect/gcc/write.c
    ${CMAKE_SOURCE_DIR}/src/asf4/stdio_redirect/gcc/read.c
    ${CMAKE_SOURCE_DIR}/src/asf4/ethernet_phy/ethernet_phy.c
    startup_samv71q21b.c
)

# Include directories
target_include_directories(
    asf4
    SYSTEM
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src/asf4/stdio_redirect
        ${CMAKE_SOURCE_DIR}/src/asf4/hri
        ${CMAKE_SOURCE_DIR}/src/asf4/hpl/core
        ${CMAKE_SOURCE_DIR}/src/asf4/hpl/gmac
        ${CMAKE_SOURCE_DIR}/src/asf4/hpl/usart
        ${CMAKE_SOURCE_DIR}/src/asf4/hpl/pio
        ${CMAKE_SOURCE_DIR}/src/asf4/hpl/pmc
        ${CMAKE_SOURCE_DIR}/src/asf4/hal/include
        ${CMAKE_SOURCE_DIR}/src/asf4/hal/utils/include
        ${CMAKE_SOURCE_DIR}/src/asf4/ethernet_phy
        ${CMAKE_SOURCE_DIR}/src/asf4/config
        ${CMAKE_SOURCE_DIR}/src/asf4/samv71b/include
        ${CMAKE_SOURCE_DIR}/src/asf4/CMSIS/Core/Include
)

target_compile_definitions(asf4 PUBLIC __SAMV71Q21B__)

target_include_directories(freertos_config INTERFACE config)

target_link_libraries(freertos_config INTERFACE asf4)

target_include_directories(csp PUBLIC shim)

add_executable(
    ${CMAKE_PROJECT_NAME}
    main.c
    initialization.c
    fault_handlers.c
    ${CMAKE_SOURCE_DIR}/src/app/hooks.c
    csp_eth_sam.c
    ${CMAKE_SOURCE_DIR}/src/drivers/simulation/siphra.c
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    asf4
    app_main
    freertos_kernel
    freertos_trace
    csp
    libparam
    params
    compiler_warnings
    # -lc
    # -lm
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include/drivers
)

set(LINKER_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/linker/samv71q21b_flash.ld)

foreach(LINKER_SCRIPT ${LINKER_SCRIPTS})
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -T ${LINKER_SCRIPT})
endforeach()

set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES LINK_DEPENDS "${LINKER_SCRIPTS}"
)

target_link_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
        # -Wl, --cref
        # --specs=nano.specs #use newlib nano
        --specs=nosys.specs #use default syscall handlers
        -Wl,--print-memory-usage
        -Wl,-Map=${CMAKE_PROJECT_NAME}.map
)

set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} SUFFIX ".elf"
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND
        ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf
        ${CMAKE_PROJECT_NAME}.bin
    COMMAND
        ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf
        ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf
)
