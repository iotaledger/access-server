/*
 * This file is part of the IOTA Access distribution
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

/****************************************************************************
* \project IOTA Access
* \file pep_plugin_print.c
* \brief
* PEP plugin that only prints on the terminal (for development purposes).
*
* @Author Bernardo Araujo
*
* \notes
*
* \history
* 17.07.2020. Initial version.
 ****************************************************************************/

#include "pep_plugin_print.h"
#include "plugin_logger.h"

#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "wallet.h"

#define RES_BUFF_LEN 80
#define MAX_ACTIONS 10
#define ACTION_NAME_SIZE 16
#define POLICY_ID_SIZE 64
#define ADDR_SIZE 128

typedef int (*action_t)(pdp_action_t* action, int should_log);

typedef struct {
  char action_names[MAX_ACTIONS][ACTION_NAME_SIZE];
  action_t actions[MAX_ACTIONS];
  size_t count;
} action_set_t;

static wallet_ctx_t* dev_wallet = NULL;
static action_set_t g_action_set;

static int log_tangle(){
  char bundle[81];
  char buf[RES_BUFF_LEN];

  wallet_send(dev_wallet,
              "MXHYKULAXKWBY9JCNVPVSOSZHMBDJRWTTXZCTKHLHKSJARDADHJSTCKVQODBVWCYDNGWFGWVTUVENB9UA",
              0,
              "hello world from access!",
              bundle);
  log_info(plugin_logger_id, "[%s:%d] Obligation of logging action to tangle. Bundle hash: %s .\n", __func__, __LINE__, bundle);
}

static int print_terminal(pdp_action_t* action){
  log_info(plugin_logger_id, "[%s:%d] Printing from PEP plugin as part of Action: %s\n", __func__, __LINE__, action->value);
  return 0;
}

static int destroy_cb(plugin_t* plugin, void* data) { free(plugin->callbacks); }

static int action_cb(plugin_t* plugin, void* data) {
  pep_plugin_args_t* args = (pep_plugin_args_t*)data;
  pdp_action_t* action = &args->action;
  char* obligation = args->obligation;
  bool should_log = FALSE;
  int status = 0;

  // handle obligations
  if (0 == memcmp(obligation, "obligation#1", strlen("obligation#1"))) {
    log_tangle();
  }

  // execute action
  for (int i = 0; i < g_action_set.count; i++) {
    if (memcmp(action->value, g_action_set.action_names[i], strlen(g_action_set.action_names[i])) == 0) {
      log_info(plugin_logger_id, "[%s:%d] Action performed: %s\n", __func__, __LINE__, action->value);
      status = g_action_set.actions[i](action, should_log);
      break;
    }
  }
  return status;
}

int pep_plugin_print_initializer(plugin_t* plugin, void* wallet_context) {
  dev_wallet = wallet_context;
  if (dev_wallet == NULL) {
    log_error(plugin_logger_id, "[%s:%d] Wallet creation failed. %s\n", __func__, __LINE__);
    return -1;
  }

  g_action_set.actions[0] = print_terminal;
  strncpy(g_action_set.action_names[0], "action#1", ACTION_NAME_SIZE);
  g_action_set.count = 1;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEP_PLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEP_PLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEP_PLUGIN_ACTION_CB] = action_cb;

  return 0;
}
