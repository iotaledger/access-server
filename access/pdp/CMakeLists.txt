set(target pdp)

add_library(${target} pdp.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC json_parser pip pap)
