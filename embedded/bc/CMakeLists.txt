set(target bc)
add_library(${target} bc_daemon.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC json_parser vehicle_dataset config_manager)
