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

set(target pep_plugins)

set(libs
  access_wallet
  pdp
  config_manager
  plugin_manager
  platform_interface)

set(sources
  can/pep_plugin_can.c
  canopen/pep_plugin_canopen.c
  wallet/pep_plugin_wallet.c
  relay/pep_plugin_relay.c
  print/pep_plugin_print.c
)

add_library(${target} ${sources})
set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/can
  ${CMAKE_CURRENT_SOURCE_DIR}/canopen
  ${CMAKE_CURRENT_SOURCE_DIR}/wallet
  ${CMAKE_CURRENT_SOURCE_DIR}/relay
  ${CMAKE_CURRENT_SOURCE_DIR}/print
)
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})
