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

set(target hsm)

# choose between st33, stsafe and riddlencode
set(hsm_implementation st33 CACHE STRING "HSM implementation choice")

if(hsm_implementation STREQUAL st33)
  set(sources st33/st33.c)
  set(hsm_include ${CMAKE_CURRENT_SOURCE_DIR}/st33)
  #set(hsm_libs xxx)
endif()

if(hsm_implementation STREQUAL stsafe)
  set(sources stsafe/stsafe.c)
  set(hsm_include ${CMAKE_CURRENT_SOURCE_DIR}/stsafe)
  #set(hsm_libs xxx)
endif()

if(hsm_implementation STREQUAL riddlencode)
  set(sources riddlencode/riddlencode.c)
  set(hsm_include ${CMAKE_CURRENT_SOURCE_DIR}/riddlencode)
  #set(hsm_libs xxx)
endif()

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${hsm_include}
)

#set(libs ${hsm_libs})

add_library(${target} ${sources})
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})
