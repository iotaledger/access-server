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

set(target cryptography)

set(sources
  sha512.c
  curve25519-donna.c
  aes.c
  sha256.c
  ref/ge25519.c
  ref/crypto_hash_sha512.c
  ref/fe25519.c
  ref/sc25519.c
  ref/keypair.c
  ref/open.c
  ref/sign.c
  ref/verify.c)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/ref)

add_library(${target} ${sources})
target_include_directories(${target} PUBLIC ${include_dirs})
