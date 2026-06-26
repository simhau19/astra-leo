FetchContent_Declare(
    libparam
    GIT_REPOSITORY https://github.com/spaceinventor/libparam.git
    GIT_TAG ${LIBPARAM_VERSION}
)

FetchContent_MakeAvailable(libparam)

option(PARAM_LIST_DYNAMIC "Enable dynamic list" OFF)
option(PARAM_HAVE_TIMESTAMP "Enable timestamp" ON)
option(PARAM_HAVE_FLOAT "Enable float support" ON)
option(PARAM_HAVE_FOPEN "Enable fopen support" OFF)
option(PARAM_VMEM_FRAM "Enable VMEM FRAM" OFF)
option(PARAM_COLLECTOR "Enable collector" OFF)
set(PARAM_LIST_POOL 0 CACHE STRING "Pool list size")

if(PARAM_LIST_DYNAMIC OR PARAM_LIST_POOL)
    set(PARAM_HAVE_SYS_QUEUE 1)
endif()

set(PARAM_VMEM_64_BITS_API 1)

if(NOT PARAM_HAVE_FLOAT)
    set(MPACK_FLOAT 0)
endif()

# Validate fopen + list_dynamic combo
if(PARAM_HAVE_FOPEN)
    if(PARAM_LIST_DYNAMIC)
        set(MPACK_STDIO 1)
    else()
        error(WARNING "Enabling 'have_fopen' without also enabling 'list_dynamic' causes mpack related build failures, setting MPACK_STDIO=0")
        set(MPACK_STDIO 0)
    endif()
endif()

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/libparam.h.in
    ${libparam_SOURCE_DIR}/include/libparam.h
)

add_library(
    libparam
    STATIC
    ${libparam_SOURCE_DIR}/src/param/list/param_list.c
    ${libparam_SOURCE_DIR}/src/param/param_client.c
    ${libparam_SOURCE_DIR}/src/param/param_serializer.c
    ${libparam_SOURCE_DIR}/src/param/param_server.c
    ${libparam_SOURCE_DIR}/src/param/param_string.c
    ${libparam_SOURCE_DIR}/src/param/param_queue.c
    ${libparam_SOURCE_DIR}/src/param/param_wildcard.c
    ${libparam_SOURCE_DIR}/src/param/param.c
    ${libparam_SOURCE_DIR}/src/mpack/mpack.c
    ${libparam_SOURCE_DIR}/src/vmem/vmem_client.c
    ${libparam_SOURCE_DIR}/src/vmem/vmem_crc32.c
    ${libparam_SOURCE_DIR}/src/vmem/vmem_server.c
    ${libparam_SOURCE_DIR}/src/vmem/vmem.c
    ${libparam_SOURCE_DIR}/src/vmem/vmem_block.c
)

if(PARAM_HAVE_FOPEN)
    target_sources(
        libparam
        ${libparam_SOURCE_DIR}/src/vmem/vmem_file.c
        ${libparam_SOURCE_DIR}/src/vmem/vmem_mmap.c
    )
endif()

if(PARAM_VMEM_FRAM)
    target_sources(
        libparam
        ${libparam_SOURCE_DIR}/src/vmem/vmem_fram.c
        ${libparam_SOURCE_DIR}/src/vmem/vmem_fram_cache.c
    )
endif()

if(PARAM_COLLECTOR)
    target_sources(
        libparam
        ${libparam_SOURCE_DIR}/src/collector/param_collector_config.c
        ${libparam_SOURCE_DIR}/src/collector/param_collector.c
    )
endif()

target_include_directories(libparam PUBLIC ${libparam_SOURCE_DIR}/include)

target_link_libraries(libparam csp freertos_kernel)
