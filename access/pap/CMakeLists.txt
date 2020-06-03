set(target pap)
add_library(${target} pap.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/validator)
target_link_libraries(${target} PUBLIC json_parser cryptography)
