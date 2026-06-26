add_library(compiler_warnings INTERFACE)

target_compile_options(
    compiler_warnings
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wcast-align
        -Wpointer-arith
        -Wwrite-strings
        -Wconversion
        -Wsign-conversion
        -Wundef
        -Wformat=2
    # -Werror
)
