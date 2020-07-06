/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \file pepplugin_wallet_demo.c
 * \brief
 * Resolver plugin for wallet functionality.
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 15.06.2020. Initial version.
 ****************************************************************************/

#include "pepplugin_wallet_demo.h"

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"
#include "dlog.h"
#include "rpi_trans.h"
#include "time_manager.h"
#include "wallet.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define TRANS_CONF_SERV_MAX_NUM 64
#define TRANS_INTERVAL_S 30
#define TRANS_TIMEOUT_S 120

#define RES_MAX_RESOLVER_ACTIONS 10
#define RES_ACTION_NAME_SIZE 16
#define RES_POL_ID_STR_LEN 64
#define RES_BUFF_LEN 80

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct serv_confirm {
  confirmation_service_t* service;
  char* policy_id;
  int policy_id_len;
  bool transaction_confirmed;
} transaction_serv_confirm_t;

typedef int (*action_t)(pdp_action_t* action, int should_log);

typedef struct {
  char action_names[RES_MAX_RESOLVER_ACTIONS][RES_ACTION_NAME_SIZE];
  action_t actions[RES_MAX_RESOLVER_ACTIONS];
  size_t count;
} action_set_t;

static wallet_ctx_t* dev_wallet = NULL;
static action_set_t actions = {0};

/****************************************************************************
 * GLOBAL VARIBLES
 ****************************************************************************/
static transaction_serv_confirm_t service[TRANS_CONF_SERV_MAX_NUM] = {0};
static pthread_mutex_t trans_mutex;

static void transaction_confirmation(uint32_t time, bool is_confirmed, pthread_t thread_id);
static bool transaction_store_transaction(char* policy_id, int policy_id_len, char* transaction_hash,
                                          int transaction_hash_len);

static int demo_wallet_transfer_tokens(pdp_action_t* action, int should_log) {
  char bundle[81];
  wallet_send(dev_wallet, action->wallet_address, action->balance, NULL, bundle);
  return 0;
}

static int demo_wallet_store_transaction(pdp_action_t* action, int should_log) {
  transaction_store_transaction(action->pol_id_str, RES_POL_ID_STR_LEN, action->transaction_hash,
                                action->transaction_hash_len);
  return 0;
}

static action_set_t g_action_set = {0};

static int destroy_cb(plugin_t* plugin, void* data) {
  free(plugin->callbacks);
  return 0;
}

static int action_cb(plugin_t* plugin, void* data) {
  pepplugin_args_t* args = (pepplugin_args_t*)data;
  pdp_action_t* action = &args->action;
  char* obligation = args->obligation;
  bool should_log = FALSE;
  char buf[RES_BUFF_LEN];
  int status = 0;

  // handle obligations
  if (0 == memcmp(obligation, "log_event", strlen("log_event"))) {
    should_log = TRUE;
  }

  // execute action
  for (int i = 0; i < g_action_set.count; i++) {
    if (memcmp(action->value, g_action_set.action_names[i], strlen(g_action_set.action_names[i])) == 0) {
      timemanager_get_time_string(buf, RES_BUFF_LEN);
      dlog_printf("%s %s\t<Action performed>\n", buf, action->value);
      status = g_action_set.actions[i](action, should_log);
      break;
    }
  }
  return status;
}

int peppluginwalletdemo_initializer(plugin_t* plugin, void* options) {
  if (plugin == NULL && options == NULL) {
    return -1;
  }

  dev_wallet = (wallet_ctx_t*)options;

  // Initalize mutex
  if (pthread_mutex_init(&trans_mutex, NULL) != 0) {
    printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
    return -1;
  }

  // Initalize mutex
  if (pthread_mutex_init(&trans_mutex, NULL) != 0) {
    printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
    return -1;
  }

  g_action_set.actions[0] = demo_wallet_transfer_tokens;
  g_action_set.actions[1] = demo_wallet_store_transaction;
  strncpy(g_action_set.action_names[0], "transfer_tokens", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[1], "store_transaction", RES_ACTION_NAME_SIZE);
  g_action_set.count = 2;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEPPLUGIN_ACTION_CB] = action_cb;

  return 0;
}

static bool transaction_store_transaction(char* policy_id, int policy_id_len, char* transaction_hash,
                                          int transaction_hash_len) {
  int i;

  // Check input parameters
  if (policy_id == NULL || transaction_hash == NULL) {
    printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
    return FALSE;
  }

  pthread_mutex_lock(&trans_mutex);

  if (!rpitransaction_store(policy_id, policy_id_len)) {
    printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
    pthread_mutex_unlock(&trans_mutex);
    return FALSE;
  }

  if (wallet_check_confirmation(dev_wallet, transaction_hash)) {
    // Confirmed transaction
    if (!rpitransaction_update_payment_status(policy_id, policy_id_len, TRUE)) {
      printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
      pthread_mutex_unlock(&trans_mutex);
      return FALSE;
    }
  } else {
    // Not confirmed.

    // Clear all confirmed services
    for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++) {
      if (service[i].transaction_confirmed) {
        pthread_join(service[i].service->thread_id, NULL);
        confirmation_service_free(&service[i].service);
        service[i].service = NULL;
        service[i].policy_id_len = 0;
        service[i].transaction_confirmed = FALSE;
        free(service[i].policy_id);
      }
    }

    // Start confirmation monitoring.
    for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++) {
      if (service[i].service == NULL) {
        // First available service
        break;
      }
    }

    if (i == TRANS_CONF_SERV_MAX_NUM) {
      printf("\nERROR[%s]: Transaction confirmation services limit reached.\n", __FUNCTION__);
      pthread_mutex_unlock(&trans_mutex);
      return FALSE;
    }

    service[i].service = confirmation_service_start(dev_wallet, transaction_hash, TRANS_INTERVAL_S, TRANS_TIMEOUT_S,
                                                    transaction_confirmation);
    service[i].policy_id = malloc(policy_id_len * sizeof(char));
    service[i].policy_id_len = policy_id_len;
    service[i].transaction_confirmed = FALSE;
  }

  pthread_mutex_unlock(&trans_mutex);
  return TRUE;
}

static void transaction_confirmation(uint32_t time, bool is_confirmed, pthread_t thread_id) {
  int i;

  // Find the service
  for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++) {
    if (service[i].service->thread_id == thread_id) {
      // Found
      break;
    }
  }

  if (i == TRANS_CONF_SERV_MAX_NUM) {
    return;
  }

  if (!rpitransaction_update_payment_status(service[i].policy_id, service[i].policy_id_len, is_confirmed)) {
    printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
    return;
  }

  service[i].transaction_confirmed = TRUE;
}
