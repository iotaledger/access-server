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

cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

project(access_wallet DESCRIPTION "Access Network Module")

enable_language(C)
enable_testing()

# libs in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

set(target network)

set(libs
  ${policy_format}
  auth
  pep
  pap_plugin_posix
  policy_updater)

add_library(${target} network.c network_logger.c)
target_include_directories(${target} PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${iota_common_SOURCE_DIR}"
)

target_link_libraries(${target} PUBLIC ${libs})
