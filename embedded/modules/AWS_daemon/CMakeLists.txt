set(target psdaemon)

add_library(${target} psDaemon.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} PUBLIC config_manager pep)
