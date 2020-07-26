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
 * \file pip_plugin_canopen.h
 * \brief
 * PIP Plugin for CAN open
 *
 * @Author Djordje Golubovic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 * 15.07.2020. Renaming
 ****************************************************************************/
#ifndef _PIP_PLUGIN_CANOPEN_H_
#define _PIP_PLUGIN_CANOPEN_H_

#include <pthread.h>
#include "vehicle_dataset.h"

#include "pip_plugin.h"
#include "plugin.h"

#ifndef bool
#define bool _Bool
#endif

int pip_plugin_canopen_initializer(plugin_t *plugin, void *data);

void canopenreceiver_init(canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex);
int canopenreceiver_start();
void canopenreceiver_deinit();
bool canopenreceiver_is_in_use();
void canopenreceiver_get_port_name(char *p_name_buff, int p_name_buff_len);
int canopenreceiver_get_node_id(void);

#endif  //_PIP_PLUGIN_CANOPEN_H_
