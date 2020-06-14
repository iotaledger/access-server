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
 * \project Decentralized Access Control
 * \file resolver.h
 * \brief
 * Implementation of Resolver
 *
 * @Author Vladimir Vojnovic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 03.10.2018. Initial version.
 * 28.02.2020. Added data sharing through action functionality
 * 14.05.2020. Refactoring
 ****************************************************************************/
#ifndef _RESOLVER_H_
#define _RESOLVER_H_

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
#define RES_MAX_RESOLVER_ACTIONS 10
#define RES_ACTION_NAME_SIZE 16
#define RES_POL_ID_STR_LEN 64

/****************************************************************************
 * TYPES & CALLBACKS
 ****************************************************************************/
typedef struct action
{
	char pol_id_str[RES_POL_ID_STR_LEN + 1];
	unsigned long start_time;
	unsigned long stop_time;
	unsigned long balance;
	char* wallet_address;
	char* transaction_hash;
	int transaction_hash_len;
	char* value;
} resolver_action_data_t;

typedef int (*resolver_action_t)(resolver_action_data_t* action, int should_log);

typedef struct
{
    char action_names[RES_MAX_RESOLVER_ACTIONS][RES_ACTION_NAME_SIZE];
    resolver_action_t actions[RES_MAX_RESOLVER_ACTIONS];
    size_t count;
    void (*init_ds_interface_cb)(Dataset_state_t*);
    void (*start_ds_interface_cb)(void);
    void (*stop_ds_interface_cb)(void);
    void (*term_ds_interface_cb)(Dataset_state_t*);
} resolver_plugin_t;

typedef void (*resolver_plugin_initializer_t)(resolver_plugin_t*, wallet_ctx_t*);
typedef void (*resolver_plugin_terminizer_t)(resolver_plugin_t*);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn  void Resolver_init(resolver_plugin_initializer_t initializer, Dataset_state_t *dstate)
 *
 * @brief   Initialize Resolver module
 *
 */
void Resolver_init(resolver_plugin_initializer_t initializer, Dataset_state_t *dstate, wallet_ctx_t* wallet_ctx);

/**
 * @fn  void Resolver_term(resolver_plugin_terminizer_t terminizer)
 *
 * @brief   Terminate Resolver module
 *
 */
void Resolver_term(resolver_plugin_terminizer_t terminizer);

#endif //_RESOLVER_H_
