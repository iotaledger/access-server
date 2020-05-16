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

#ifndef TINY_EMBEDDED
void CanopenReceiver_preInitSetup();
#endif
void CanopenReceiver_init(canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex);
int CanopenReceiver_start();
void CanopenReceiver_deinit();
bool CanopenReceiver_isInUse();
void CanopenReceiver_getPortName(char* p_name_buff, int p_name_buff_len);
int CanopenReceiver_getNodeId(void);

#endif
