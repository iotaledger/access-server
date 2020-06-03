cmake_minimum_required(VERSION 3.10)

project(Access)

include_directories(embedded/include plugins/data/dataset)

add_subdirectory(embedded/CpuGeneric/jsmn)
add_subdirectory(embedded/CpuGeneric/crypto)
add_subdirectory(embedded/CpuGeneric/libfastjson)
add_subdirectory(embedded/include)
add_subdirectory(plugins)
add_subdirectory(embedded/bc)
add_subdirectory(embedded/modules/AWS_daemon)

add_subdirectory(access/pdp)
add_subdirectory(access/pip)
add_subdirectory(access/pap)
add_subdirectory(access/pep)

add_subdirectory(network)
add_subdirectory(network/asn)

add_subdirectory(embedded/config_manager)

add_subdirectory(embedded)
