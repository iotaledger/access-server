/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CANOPEN_RECEIVER_H_
#define _CANOPEN_RECEIVER_H_

#include <pthread.h>
#include "vehicle_dataset.h"

#define bool _Bool

void CanopenReceiver_init(canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex, const char *can_interface_name, int node_id);
int CanopenReceiver_start();
void CanopenReceiver_deinit();
bool CanopenReceiver_isInUse();
void CanopenReceiver_getPortName(char* p_name_buff, int p_name_buff_len);
int CanopenReceiver_getNodeId(void);

#endif
