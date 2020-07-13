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

set(target asn)

set(asn_implementation tinyembedded CACHE STRING "ASN implementation choice")

set(libs json_parser cryptography)

# Depends on system libcrypto for libCrypto (ssl) variant
if(asn_implementation STREQUAL libCrypto)
set(libs ${libs} -lcrypto)
endif()

set(sources
  asn_auth.c
  asn_auth_helper.c
  implementations/${asn_implementation}/asn_client.c
  implementations/${asn_implementation}/asn_server.c
  implementations/${asn_implementation}/asn_client.c
  implementations/${asn_implementation}/asn_utils.c)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/implementations/${asn_implementation})

add_library(${target} ${sources})
target_include_directories(${target} PUBLIC ${include_dirs})
target_link_libraries(${target} PUBLIC ${libs})
