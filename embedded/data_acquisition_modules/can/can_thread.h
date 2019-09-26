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
