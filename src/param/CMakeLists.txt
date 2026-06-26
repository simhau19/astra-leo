add_library(params OBJECT ./csp_debug_param.c ./siphra_params.c ./params.c)

target_include_directories(params PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(params PRIVATE libparam csp app_main)
