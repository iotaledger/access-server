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
