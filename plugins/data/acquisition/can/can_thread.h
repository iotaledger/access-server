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
 * \file can_thread.h
 * \brief
 * CAN reader thread header
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/
#ifndef _CAN_THREAD_H_
#define _CAN_THREAD_H_

#include <pthread.h>
#include "can_linux.h"

typedef struct {
    char can_bus_name[64];
    CAN_t can_connection;
    pthread_t thread;
    void (*can_frame_read_cb)(struct can_frame *frame);
} CanThread_instance_t;

void CanThread_init(CanThread_instance_t* inst, const char* can_bus_name, void (*can_cb)(struct can_frame *frame));
int CanThread_start(CanThread_instance_t* inst);
int CanThread_stop(CanThread_instance_t* inst);

#endif
