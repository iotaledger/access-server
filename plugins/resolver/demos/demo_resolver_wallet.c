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
 * \file demo_resolver_wallet.c
 * \brief
 * Resolver plugin for wallet functionality.
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 15.06.2020. Initial version.
 ****************************************************************************/

#include "demo_resolver_wallet.h"

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"
#include "transaction.h"
#include "wallet.h"

static wallet_ctx_t* dev_wallet = NULL;

static int demo_wallet_transfer_tokens(resolver_action_data_t* action, int should_log) {
  char bundle[81];
  wallet_send(dev_wallet, action->wallet_address, action->balance, NULL, bundle);
  return 0;
}

static int demo_wallet_store_transaction(resolver_action_data_t* action, int should_log) {
  transaction_store_transaction(action->pol_id_str, RES_POL_ID_STR_LEN, action->transaction_hash,
                                action->transaction_hash_len);
  return 0;
}

static resolver_plugin_t* g_action_set = NULL;

static void init_ds_interface(dataset_state_t* vdstate) {}

static void start_ds_interface() {}

static void stop_ds_interface() {}

static void term_ds_interface(dataset_state_t* vdstate) {}

void demowalletplugin_initializer(resolver_plugin_t* action_set) {
  if (action_set == NULL) {
    return;
  }

  if (action_set != NULL) {
    g_action_set = action_set;
  }

  dev_wallet = wallet_create(NODE_URL, NODE_PORT, NULL, NODE_DEPTH, NODE_MWM, WALLET_SEED);

  g_action_set->actions[0] = demo_wallet_transfer_tokens;
  g_action_set->actions[1] = demo_wallet_store_transaction;
  strncpy(g_action_set->action_names[0], "transfer_tokens", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set->action_names[1], "store_transaction", RES_ACTION_NAME_SIZE);
  g_action_set->count = 2;
  g_action_set->init_ds_interface_cb = init_ds_interface;
  g_action_set->stop_ds_interface_cb = stop_ds_interface;
  g_action_set->start_ds_interface_cb = start_ds_interface;
  g_action_set->term_ds_interface_cb = term_ds_interface;
}

void demowalletplugin_terminizer() { 
  g_action_set = NULL; 
  dev_wallet = NULL;
}
