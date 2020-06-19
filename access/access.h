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

#ifndef _ACCESS_H_
#define _ACCESS_H_

#include "dataset.h"
#include "wallet.h"
#include "pep_plugin.h"
#include "pap_plugin.h"
#include "pip_plugin.h"

typedef void *access_ctx_t;

void access_init(access_ctx_t *access_context, pep_plugin_initializer_t pep_plugin_init_fn[], pip_plugin_initializer_t pip_plugin_init_fn[]);
void access_start(access_ctx_t access_context);
void access_deinit(access_ctx_t access_context);
void access_get_ddstate(access_ctx_t access_context, dataset_state_t **ddstate);

#endif
