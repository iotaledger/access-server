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
 * \file can_receiver.h
 * \brief
 * CAN receiver server header file
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#ifndef _CAN_RECEIVER_H_
#define _CAN_RECEIVER_H_

#include <pthread.h>
#include <libfastjson/json.h>
#include "vehicle_dataset.h"
#include "can_thread.h"

void CanReceiver_init(const char* can_body_channel, const char* can_chas_channel, can01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex);
void CanReceiver_start();
int CanReceiver_deinit();

#endif
