set(target fastjson)

add_library(${target} arraylist.c debug.c json_object.c json_object_iterator.c json_print.c json_tokener.c json_util.c json_version.c printbuf.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
