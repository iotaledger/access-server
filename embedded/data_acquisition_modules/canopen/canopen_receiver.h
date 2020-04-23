/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \file canopen_receiver.h
 * \brief
 * Implementation of interface for CANOPEN receiver
 *
 * @Author
 *
 * \notes
 *
 * \history
 * XX.YY.ZZZZ. Initial version.
 ****************************************************************************/
#ifndef _CANOPEN_RECEIVER_H_
#define _CANOPEN_RECEIVER_H_

#include <pthread.h>
#include "vehicle_dataset.h"

#ifndef bool
#define bool _Bool
#endif

typedef enum {
	CANOPEN_SHD_INITIALIZING = 0x00,
	CANOPEN_SHD_STOPPED = 0x04,
	CANOPEN_SHD_OPERATIONAL = 0x05,
	CANOPEN_SHD_PREOPERATIONAL = 0x7f
} Canopen_service_heartbeat_data_e;

typedef enum {
	CANOPEN_STC_4BYTES = 0x43,
	CANOPEN_STC_3BYTES = 0x47,
	CANOPEN_STC_2BYTES = 0x4b,
	CANOPEN_STC_1BYTE = 0x4f
} Canopen_service_tsdo_command_e;

#ifndef TINY_EMBEDDED
void CanopenReceiver_preInitSetup(const char *can_interface_name, int _node_id);
#endif
void CanopenReceiver_init(canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex, const char *can_interface_name, int node_id);
int CanopenReceiver_start();
void CanopenReceiver_deinit();
bool CanopenReceiver_isInUse();
void CanopenReceiver_getPortName(char* p_name_buff, int p_name_buff_len);
int CanopenReceiver_getNodeId(void);

#endif
