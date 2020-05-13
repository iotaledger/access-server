/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>

typedef int32_t wallet_err_t;

#define WALLET_OK 0
#define WALLET_ERR_UNKNOW -1
#define WALLET_ERR_INVALID_PRARMS 1   // invalid paramiters
#define WALLET_ERR_OOM 1 << 1         // out of memory
#define WALLET_ERR_CLIENT_ERR 1 << 2  // cclient error
#define WALLET_ERR_FLEX_TRITS 1 << 3  // flex trits error
