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

#include "access.h"

#include <stdlib.h>
#include <string.h>

#include "config_manager.h"
#include "datadumper.h"
#include "pep.h"
#include "policy_loader.h"

#include "protocol.h"
#include "timer.h"
#include "transaction.h"

#define MAX_CLIENT_NAME 32
#define CONFIG_CLIENT_CAN01 "can01"
#define CONFIG_CLIENT_CANOPEN01 "canopen01"
#define CONFIG_CLIENT_OBDII "obdii"

typedef struct {
  pthread_mutex_t *json_mutex;
  dataset_state_t ddstate;
} access_ctx_t_;

void access_init(access_ctx_t *access_context,
                 pep_plugin_initializer_t pep_plugin_init_fn[],
                 pip_plugin_initializer_t pip_plugin_init_fn[]){

  access_ctx_t_ *ctx = calloc(1, sizeof(access_ctx_t_));

  // Register plugins
  timer_init();
  pap_plugin_init();

  pep_init();

  ctx->json_mutex = datadumper_get_mutex();

  for (int i = 0; i < PEP_MAX_ACT_CALLBACKS; i++){
    if (pep_plugin_init_fn[i] == NULL) break;
    pep_register_callback(i, pep_plugin_init(pep_plugin_init_fn[i], &ctx->ddstate));
  }

  for (int i = 0; i < PIP_MAX_PROBE_CALLBACKS; i++){
    if (pip_plugin_init_fn[i] == NULL) break;
    pip_register_callback(i, pip_plugin_init(pip_plugin_init_fn[i], &ctx->ddstate));
  }

  *access_context = (access_ctx_t)ctx;
  datadumper_init();
}

void access_start(access_ctx_t access_context) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;

  policyloader_start();
}

void access_deinit(access_ctx_t access_context) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;

  policyloader_stop();

  timer_deinit();
}

void access_get_ddstate(access_ctx_t access_context, dataset_state_t **ddstate) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;
  *ddstate = &ctx->ddstate;
}
