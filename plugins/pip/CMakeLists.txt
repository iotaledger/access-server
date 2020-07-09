#
# This file is part of the IOTA Access distribution
# (https://github.com/iotaledger/access)
#
# Copyright (c) 2020 IOTA Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.11)

set(target pip_plugins)

set(libs
  -pthread
  access_wallet
  pdp
  pluginmanager
  json_parser
  pip
  access_wallet
  vehicle_dataset
  config_manager
  platform_interface)

set(sources
  can/can_linux.c
  can/can_msgs.c
  can/can_receiver.c
  can/can_thread.c
  can/lib.c
  canopen/canopen_receiver.c
  canopen/canopen_sdo.c
  gps/gps_receiver.c
  gps/minmea/minmea.c
  modbus/modbus.c
  modbus/modbus_receiver.c
  obdii/obdii_receiver.c
  obdii/ejv_obdii/OBDII.c
  obdii/ejv_obdii/OBDIICommunication.c
  obdii/ejv_obdii/OBDIIDaemon.c
  wallet/pip_plugin_wallet.c
)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/can
  ${CMAKE_CURRENT_SOURCE_DIR}/canopen
  ${CMAKE_CURRENT_SOURCE_DIR}/gps
  ${CMAKE_CURRENT_SOURCE_DIR}/gps/minmea
  ${CMAKE_CURRENT_SOURCE_DIR}/obdii
  ${CMAKE_CURRENT_SOURCE_DIR}/obdii/ejv_obdii
  ${CMAKE_CURRENT_SOURCE_DIR}/wallet
)

add_library(${target} ${sources})
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})
