set(TARGET_ARCH "cortex-m7")
set(TARGET_CHIP "samv71q21b")
set(TARGET_BOARD "samv71_xultra")

set(FREERTOS_PORT GCC_ARM_CM7)

set(MCU_FLAGS
    -mcpu=${TARGET_ARCH}
    -mthumb
    #enable floating point unit (FPU)
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

set(COMPILER_FLAGS
    -ffunction-sections
    -fdata-sections
    -fno-common
    -fmessage-length=0
)

set(LINKER_FLAGS -Wl,--gc-sections)

add_compile_options(${MCU_FLAGS} ${COMPILER_FLAGS})

add_link_options(${MCU_FLAGS} ${LINKER_FLAGS})
