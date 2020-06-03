set(target legacy_modules)

add_library(${target} tcp_client.c utils_string.c time_manager.c timer.c json_interface.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC config_manager)
