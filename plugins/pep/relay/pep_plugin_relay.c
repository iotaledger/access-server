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

#include "pep_plugin_relay.h"
#include "plugin_logger.h"

#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "relay_interface.h"
#include "wallet.h"

#define RES_BUFF_LEN 80
#define MAX_ACTIONS 10
#define ACTION_NAME_SIZE 16
#define POLICY_ID_SIZE 64
#define ADDR_SIZE 128
#define ACTION_ADDRESS "MXHYKULAXKWBY9JCNVPVSOSZHMBDJRWTTXZCTKHLHKSJARDADHJSTCKVQODBVWCYDNGWFGWVTUVENB9UA"
#define ACTION_MSG_MAX_SIZE 512

typedef int (*action_t)(pdp_action_t* action);

typedef struct {
  char action_names[MAX_ACTIONS][ACTION_NAME_SIZE];
  action_t actions[MAX_ACTIONS];
  size_t count;
} action_set_t;

static wallet_ctx_t* dev_wallet = NULL;
static action_set_t g_action_set;

static int log_tangle(char* msg) {
  char bundle_hash[NUM_TRYTES_BUNDLE + 1] = {};

  wallet_err_t ret = wallet_send(dev_wallet, ACTION_ADDRESS, 0, msg, bundle_hash);

  bundle_hash[NUM_TRYTES_BUNDLE] = '\0';

  if (ret != WALLET_OK) {
    log_error(plugin_logger_id, "[%s:%d] Could not log action to Tangle. %s\n", __func__, __LINE__);
    return -1;
  }

  log_info(plugin_logger_id, "[%s:%d] logging Action to Tangle. Bundle hash: %s\n", __func__, __LINE__, bundle_hash);

  return 0;
}

static int relay_on() {
  int relay_index = 0;
  relayinterface_on(relay_index);

  // log action on Tangle
  char msg[ACTION_MSG_MAX_SIZE] = {};
  sprintf(msg, "Relay %d ON.", relay_index);
  log_tangle(msg);

  // log action on Terminal
  log_info(plugin_logger_id, "[%s:%d] Relay %d ON.\n", __func__, __LINE__, relay_index);

  return 0;
}

static int relay_off() {
  int relay_index = 0;
  relayinterface_off(relay_index);

  // log action on Tangle
  char msg[ACTION_MSG_MAX_SIZE] = {};
  sprintf(msg, "Relay %d OFF.", relay_index);
  log_tangle(msg);

  // log action on Terminal
  log_info(plugin_logger_id, "[%s:%d] Relay %d OFF.\n", __func__, __LINE__, relay_index);

  return 0;
}

static int destroy_cb(plugin_t* plugin, void* data) { free(plugin->callbacks); }

static int action_cb(plugin_t* plugin, void* data) {
  pep_plugin_args_t* args = (pep_plugin_args_t*)data;
  pdp_action_t* action = &args->action;
  char* obligation = args->obligation;
  char buf[RES_BUFF_LEN];
  int status = 0;

  // handle obligations
  if (0 == memcmp(obligation, "obligation#1", strlen("obligation#1"))) {
    // obligation here
  }

  // execute action
  for (int i = 0; i < g_action_set.count; i++) {
    if (memcmp(action->value, g_action_set.action_names[i], strlen(g_action_set.action_names[i])) == 0) {
      log_info(plugin_logger_id, "[%s:%d] Action performed: %s\n", __func__, __LINE__, action->value);
      status = g_action_set.actions[i](action);
      break;
    }
  }

  return status;
}

int pep_plugin_relay_initializer(plugin_t* plugin, void* wallet_context) {
  dev_wallet = wallet_context;
  if (dev_wallet == NULL) {
    log_error(plugin_logger_id, "[%s:%d] Wallet creation failed. %s\n", __func__, __LINE__);
    return -1;
  }

  g_action_set.actions[0] = relay_on;
  g_action_set.actions[1] = relay_off;
  strncpy(g_action_set.action_names[0], "action#1", ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[1], "action#2", ACTION_NAME_SIZE);
  g_action_set.count = 2;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEP_PLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEP_PLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEP_PLUGIN_ACTION_CB] = action_cb;

  return 0;
}

