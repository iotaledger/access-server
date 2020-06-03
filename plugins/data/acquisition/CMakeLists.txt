set(target data_acquisition_plugins)

set(can_plugin_sources can/can_linux.c can/can_msgs.c can/can_receiver.c can/can_thread.c can/lib.c)
set(canopen_plugin_sources canopen/canopen_receiver.c canopen/canopen_sdo.c)
set(gps_plugin_sources gps/gps_receiver.c gps/minmea/minmea.c)
set(obdii_plugin_sources obdii/obdii_receiver.c obdii/ejv_obdii/OBDII.c obdii/ejv_obdii/OBDIICommunication.c obdii/ejv_obdii/OBDIIDaemon.c)
set(modbus_plugin_sources modbus/modbus.c modbus/modbus_receiver.c)

add_library(${target} protocol.c ${can_plugin_sources} ${canopen_plugin_sources} ${gps_plugin_sources} ${obdii_plugin_sources} ${modbus_plugin_sources})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/can ${CMAKE_CURRENT_SOURCE_DIR}/canopen ${CMAKE_CURRENT_SOURCE_DIR}/gps ${CMAKE_CURRENT_SOURCE_DIR}/obdii ${CMAKE_CURRENT_SOURCE_DIR}/gps/minmea ${CMAKE_CURRENT_SOURCE_DIR}/obdii/ejv_obdii ${CMAKE_CURRENT_SOURCE_DIR}/modbus)
target_link_libraries(${target} PUBLIC json_parser pip vehicle_dataset config_manager)
