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

set(target pap)

set(user_management_implementation sqlite3_example CACHE STRING
  "User management implementation choice")

set(sources
  pap.c
  user/user.c
  user/implementations/${user_management_implementation}/user_implementation.c
  validator/parser.c
  validator/validator.c)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/validator
  ${CMAKE_CURRENT_SOURCE_DIR}/user
  ${CMAKE_CURRENT_SOURCE_DIR}/user/implementations/${user_management_implementation})

set(libs
  json_parser
  cryptography
  config_manager)

add_library(${target} ${sources})
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})
