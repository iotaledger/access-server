set(target network)

add_library(${target} network.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC json_parser asn pep storage_plugin)
