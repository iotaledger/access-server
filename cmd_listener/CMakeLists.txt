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

set(sources
        cmd_listener.c
        cmd_listener_logger.c
        auth/auth_cmd_listener.c
        decision/cmd_decision.c)

set(libs auth pep pthread)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/auth
  ${CMAKE_CURRENT_SOURCE_DIR}/iota
  ${CMAKE_CURRENT_SOURCE_DIR}/decision)

add_executable(acl ${sources})
add_dependencies(acl auth)

target_include_directories(acl PUBLIC ${include_dirs})
target_link_directories(acl PUBLIC ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(acl PUBLIC ${libs})