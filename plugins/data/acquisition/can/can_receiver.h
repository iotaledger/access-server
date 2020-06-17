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

#include "can_thread.h"
#include "vehicle_dataset.h"

#ifndef bool
#define bool _Bool
#endif

typedef enum { CAN_DOOR_UNKNOWN = 0x00, CAN_DOOR_OPENED = 0x01, CAN_DOOR_CLOSED = 0x02 } can_door_status_e;

typedef enum {
  CAN_LOCK_UNDEF = 0x00,
  CAN_LOCK_OPENED = 0x01,
  CAN_LOCK_CLOSED = 0x02,
  CAN_LOCK_LOCKED = 0x03,
  CAN_LOCK_SAFE = 0x04
} can_lock_status_e;

typedef enum { CAN_TEMP_UNIT_C = 0x00, CAN_TEMP_UNIT_F = 0x01, CAN_TEMP_UNIT_UNKNOWN = 0x02 } can_temp_unit_e;

#ifndef TINY_EMBEDDED
void canreceiver_pre_init_setup();
#endif
void canreceiver_init(can01_vehicle_dataset_t* dataset, pthread_mutex_t* json_mutex);
void canreceiver_start();
int canreceiver_deinit();
bool canreceiver_is_in_use();
void canreceiver_get_body_channel(char* channel_buff, int channel_buff_len);
void canreceiver_get_chas_channel(char* channel_buff, int channel_buff_len);

#endif
