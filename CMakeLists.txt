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

set(policy_format json CACHE STRING "Policy Format Choice")
#set(policy_format protobuf-c CHACE STRING "Policy Format Choice")

# fetch pigpio
include(FetchContent)
FetchContent_Declare(
  pigpio
  GIT_REPOSITORY https://github.com/joan2937/pigpio.git
  GIT_TAG b6fe5874516eb67745fa6277d2060b715dac09a9
)

# fetch sqlite-amalgamation
include(FetchContent)
FetchContent_Declare(
  sqlite3
  GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation.git
  GIT_TAG 9be05e13408dd8cf8dafe8f11b1700a28194e07e
)

# fetch iota_common
include(FetchContent)
FetchContent_Declare(
  iota_common
  GIT_REPOSITORY http://github.com/iotaledger/iota_common.git
  GIT_TAG cf649803757abf48432d4fa60e9f27ac119bae5f
)

# fetch iota.c
include(FetchContent)
FetchContent_Declare(
  iota.c
  GIT_REPOSITORY http://github.com/iotaledger/iota.c.git
  GIT_TAG 5d4433f7b936822f7d4c5bfc4f3e25899f7a3ce8
)

if(${CMAKE_VERSION} VERSION_LESS 3.14)
    macro(FetchContent_MakeAvailable NAME)
        FetchContent_GetProperties(${NAME})
        if(NOT ${NAME}_POPULATED)
            FetchContent_Populate(${NAME})
            add_subdirectory(${${NAME}_SOURCE_DIR} ${${NAME}_BINARY_DIR})
        endif()
    endmacro()
endif()

message(STATUS "Fetching pigpio")
FetchContent_MakeAvailable(pigpio)

message(STATUS "Fetching sqlite3")
FetchContent_MakeAvailable(sqlite3)

message(STATUS "Fetching iota_common")
FetchContent_MakeAvailable(iota_common)

message(STATUS "Fetching iota.c")
FetchContent_MakeAvailable(iota.c)

# fetch external libs
include(ExternalProject)

set(target asri)

set(libs
  sqlite3
  ${policy_format}
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
  ${CMAKE_CURRENT_SOURCE_DIR}/policy_loader
  ${CMAKE_CURRENT_SOURCE_DIR}/config_manager
)

add_executable(${target} main.c)
target_include_directories(${target} PUBLIC ${include_dirs})

target_link_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ext_install/lib)

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
