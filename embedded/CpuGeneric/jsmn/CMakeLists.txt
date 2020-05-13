set(target json_parser)
add_library(${target} jsmn.c json_parser.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
