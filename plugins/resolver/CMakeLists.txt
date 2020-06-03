set(target resolver)

add_library(${target} resolver.c platform_interface/r_pi/relay_interface.c demos/demo_resolver_01.c demos/demo_resolver_02.c)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/platform_interface/r_pi ${CMAKE_CURRENT_SOURCE_DIR}/demos)
target_link_libraries(${target} PUBLIC pep data_acquisition_plugins pap)
