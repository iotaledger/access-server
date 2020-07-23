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

# fetch access-sdk
include(FetchContent)
FetchContent_Declare(
  access-sdk
  GIT_REPOSITORY https://github.com/iotaledger/access-sdk
  GIT_TAG bbb7ee98449d41b20ca9d71152f8f6cfe50269c8
)

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

message(STATUS "Fetching access-sdk")
FetchContent_MakeAvailable(access-sdk)

message(STATUS "Fetching pigpio")
FetchContent_MakeAvailable(pigpio)

message(STATUS "Fetching sqlite3")
FetchContent_MakeAvailable(sqlite3)

message(STATUS "Fetching iota.c")
FetchContent_MakeAvailable(iota.c)

# fetch external libs
include(ExternalProject)

set(target acsri)

set(libs
  sqlite3
  json_parser
  vehicle_dataset
  policy_loader
  network
  data_dumper
  misc
  access_core
  pep_plugin_print
)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/policy_loader
  ${CMAKE_CURRENT_SOURCE_DIR}/config_manager
)

add_executable(${target} main.c)
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.ini
  ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wallet_pem.txt
  ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
)

add_subdirectory(tests)
add_subdirectory(plugins)
add_subdirectory(network)
add_subdirectory(config_manager)
add_subdirectory(data_dumper)
add_subdirectory(policy_loader)
add_subdirectory(policy_updater)
