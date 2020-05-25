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

# Config manager implementation choice.
if(NOT DEFINED ${config_manager_impl})
set(config_manager_impl ini)
endif()

set(target config_manager)

set(source_files config_manager.c impls/${config_manager_impl}/config_manager_impl.c)

add_library(${target} ${source_files})
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/impls/${config_manager_impl})

# Config manager implementation test
set(target config_manager_test)
add_executable(${target} impls/${config_manager_impl}/test.c)
target_link_libraries(${target} PUBLIC config_manager)
