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
