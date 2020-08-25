#
# This file is part of the IOTA Access distribution
# (https://github.com/iotaledger/access)
#
# Copyright (c) 2020 IOTA Stiftung
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

project(access_reference_implementation)

set(CMAKE_C_STANDARD 99)

if(NOT DEFINED POLICY_FORMAT)
    set(POLICY_FORMAT json CACHE STRING "Policy Format Choice")
    #set(POLICY_FORMAT protobuf-c CHACE STRING "Policy Format Choice")
endif()

if(NOT DEFINED AUTH_FLAVOUR)
    #set(AUTH_FLAVOUR rsa CACHE STRING "auth flavour choice")
    set(AUTH_FLAVOUR eddsa CACHE STRING "auth flavour choice")
endif()

include(cmake/pigpio.cmake)
include(cmake/sqlite3.cmake)
include(cmake/iota_common.cmake)
include(cmake/iota.c.cmake)

# submodule while private
#include(cmake/access-sdk.cmake)

set(target asri)

set(libs
  auth
  sqlite3
  ${POLICY_FORMAT}
  vehicle_dataset
  policy_loader
  network
  data_dumper
  misc
  access_core
)

set(plugins
  pep_plugin_print
  pap_plugin_posix
)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/auth
  ${CMAKE_CURRENT_SOURCE_DIR}/policy_loader
  ${CMAKE_CURRENT_SOURCE_DIR}/config_manager
)

add_executable(${target} main.c)
target_include_directories(${target} PUBLIC ${include_dirs})

target_link_directories(${target} PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}/ext_install/lib
  ${CMAKE_CURRENT_BINARY_DIR}/access-sdk/auth
  ${CMAKE_CURRENT_BINARY_DIR}/access-sdk/auth/${AUTH_FLAVOUR})
message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/access-sdk/auth/${AUTH_FLAVOUR})

target_link_libraries(${target} PUBLIC
  ${libs}
  ${plugins}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.ini
  ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wallet_pem.txt
  ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
)

add_subdirectory(access-sdk)
add_subdirectory(portability)
add_subdirectory(tests)
add_subdirectory(network)
add_subdirectory(plugins)
add_subdirectory(config_manager)
add_subdirectory(data_dumper)
add_subdirectory(policy_loader)
add_subdirectory(policy_updater)
add_subdirectory(wallet)
add_subdirectory(access)
