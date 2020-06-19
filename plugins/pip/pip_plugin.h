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
 * \file pip_plugin.h
 * \brief
 * Implementation of PIP Plugin
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
#ifndef _PIP_PLUGIN_H_
#define _PIP_PLUGIN_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdlib.h>
#include "dataset.h"
#include "pip.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define MAX_STR_SIZE 256
#define MAX_PIP_PLUGIN_PROBES 10
#define PROBE_NAME_SIZE 16
#define POL_ID_STR_LEN 64

/****************************************************************************
 * TYPES & CALLBACKS
 ****************************************************************************/
typedef struct probe {
  char pol_id_str[POL_ID_STR_LEN + 1];
  unsigned long balance;
  char* wallet_address;
  char* transaction_hash;
  int transaction_hash_len;
  char* value;
} pip_plugin_probe_data_t;

typedef int (*pip_plugin_probe_t)(pip_plugin_probe_data_t* probe, int should_log);

typedef struct {
  char probe_names[MAX_PIP_PLUGIN_PROBES][PROBE_NAME_SIZE];
  pip_plugin_probe_t probes[MAX_PIP_PLUGIN_PROBES];
  size_t count;
  
  void (*init_ds_interface_cb)(dataset_state_t*);
  void (*start_ds_interface_cb)(void);
  void (*stop_ds_interface_cb)(void);
  void (*term_ds_interface_cb)(dataset_state_t*);
} pip_plugin_t;

typedef void (*pip_plugin_initializer_t)(pip_plugin_t*);
typedef void (*pip_plugin_terminator_t)(pip_plugin_t*);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn  void pip_plugin_init(pip_plugin_initializer_t initializer, dataset_state_t *dstate)
 *
 * @brief   Initialize pip_plugin module
 *
 */
pip_plugin_fn pip_plugin_init(pip_plugin_initializer_t initializer, dataset_state_t* dstate);

/**
 * @fn  void pip_plugin_term(pip_plugin_terminizer_t terminator)
 *
 * @brief   Terminate pip_plugin module
 *
 */
void pip_plugin_term(pip_plugin_terminator_t terminator);

#endif  //_PIP_PLUGIN_H_