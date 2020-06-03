set(target main)
add_executable(${target} main.c)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${target} -pthread -lcurl -lsqlite3 json_parser pep pdp asn bc data_acquisition_plugins vehicle_dataset resolver pap network psdaemon legacy_modules fastjson)
