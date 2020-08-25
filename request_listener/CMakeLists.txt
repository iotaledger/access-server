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

#add_subdirectory(auth)
#add_subdirectory(iota)

set(sources
  request_listener.c
  auth/auth_request_listener.c)
set(libs auth pthread)

add_executable(arl ${sources})
add_dependencies(arl auth)

target_include_directories(arl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/auth ${AUTH_SOURCE_DIR}/${AUTH_FLAVOUR})
target_link_directories(arl PUBLIC ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(arl PUBLIC ${libs})