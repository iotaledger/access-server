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
 * Implementation of Resolver for Raspberry Pi 3B+ board
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
#include "vehicle_dataset.h"
#include "vehicle_datasharing_dataset.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define RES_MAX_STR_SIZE 256
#define RES_MAX_RESOLVER_ACTIONS 10
#define RES_RESOLVER_ACTION_NAME_SIZE 16

/****************************************************************************
 * TYPES & CALLBACKS
 ****************************************************************************/
typedef int (*resolver_action_t)(int should_log);

typedef struct
{
	char action_names[RES_MAX_RESOLVER_ACTIONS][RES_RESOLVER_ACTION_NAME_SIZE];
    resolver_action_t actions[RES_MAX_RESOLVER_ACTIONS];
    size_t count;
    void (*init_ds_interface_cb)(VehicleDataset_state_t*);
    void (*start_ds_interface_cb)(void);
    void (*stop_ds_interface_cb)(void);
    void (*term_ds_interface_cb)(void);
} resolver_plugin_t;

typedef void (*resolver_plugin_initializer_t)(resolver_plugin_t*);
typedef void (*resolver_plugin_terminizer_t)(resolver_plugin_t*);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn  void Resolver_init(resolver_plugin_initializer_t initializer, VehicleDataset_state_t *vdstate)
 *
 * @brief   Initialize Resolver module
 *
 */
void Resolver_init(resolver_plugin_initializer_t initializer, VehicleDataset_state_t *vdstate);

/**
 * @fn  void Resolver_term(resolver_plugin_terminizer_t terminizer)
 *
 * @brief   Terminate Resolver module
 *
 */
void Resolver_term(resolver_plugin_terminizer_t terminizer);

#endif //_RESOLVER_H_
