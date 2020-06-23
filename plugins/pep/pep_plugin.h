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

/****************************************************************************
 * \project IOTA Access
 * \file pep_plugin.h
 * \brief
 * Implementation of PEP Plugin
 *
 * @Author Vladimir Vojnovic, Strahinja Golic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 03.10.2018. Initial version.
 * 28.02.2020. Added data sharing through action functionality
 * 14.05.2020. Refactoring
 ****************************************************************************/
#ifndef _PEP_PLUGIN_H_
#define _PEP_PLUGIN_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdlib.h>
#include "dataset.h"
#include "wallet.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define RES_MAX_STR_SIZE 256
#define RES_MAX_PEP_PLUGIN_ACTIONS 10
#define RES_ACTION_NAME_SIZE 16
#define RES_POL_ID_STR_LEN 64

/****************************************************************************
 * TYPES & CALLBACKS
 ****************************************************************************/
typedef struct action {
  char pol_id_str[RES_POL_ID_STR_LEN + 1];
  unsigned long start_time;
  unsigned long stop_time;
  unsigned long balance;
  char* wallet_address;
  char* transaction_hash;
  int transaction_hash_len;
  char* value;
} pep_plugin_action_data_t;

typedef int (*pep_plugin_action_t)(pep_plugin_action_data_t* action, int should_log);

typedef struct {
  char action_names[RES_MAX_PEP_PLUGIN_ACTIONS][RES_ACTION_NAME_SIZE];
  pep_plugin_action_t actions[RES_MAX_PEP_PLUGIN_ACTIONS];
  size_t count;
  void (*init_ds_interface_cb)(dataset_state_t*);
  void (*start_ds_interface_cb)(void);
  void (*stop_ds_interface_cb)(void);
  void (*term_ds_interface_cb)(dataset_state_t*);
} pep_plugin_t;

typedef void (*pep_plugin_initializer_t)(pep_plugin_t*, void*);
typedef void (*pep_plugin_terminator_t)(pep_plugin_t*);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn  void pep_plugin_init(pep_plugin_initializer_t initializer, Dataset_state_t *dstate)
 *
 * @brief   Initialize pep_plugin module
 *
 */
void pep_plugin_init(pep_plugin_initializer_t initializer, dataset_state_t* dstate, void* options);

/**
 * @fn  void pep_plugin_term(pep_plugin_terminator_t terminator)
 *
 * @brief   Terminate pep_plugin module
 *
 */
void pep_plugin_term(pep_plugin_terminator_t terminator);

#endif  //_PEP_PLUGIN_H_
