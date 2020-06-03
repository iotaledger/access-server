set(target storage_plugin)
add_library(${target} storage.c platforms/r_pi/rpi_storage.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/platforms/r_pi)
target_link_libraries(${target} PUBLIC pap)
