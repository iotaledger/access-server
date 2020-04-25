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
 * \file modbus_receiver.h
 * \brief
 * Modbus receiver module interface
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.29.2019. Initial version.
 ****************************************************************************/

#ifndef _MODBUS_RECEIVER_H_
#define _MODBUS_RECEIVER_H_

#include <pthread.h>
#include <libfastjson/json.h>
#include "vehicle_dataset.h"
#include "modbus.h"

void ModbusReceiver_init(const char* serial_device, canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex);
int ModbusReceiver_start();
void ModbusReceiver_stop();

#endif
