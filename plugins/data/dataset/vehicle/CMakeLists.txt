set(target vehicle_dataset)
add_library(${target} vehicle_dataset.c vehicle_datasharing_dataset.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC json_parser)
