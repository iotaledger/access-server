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
 * \file can_linux.h
 * \brief
 * Implementation of interface for linux CAN
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * XX.YY.ZZZZ. Initial version.
 ****************************************************************************/
#ifndef _CAN_LINUX_H_
#define _CAN_LINUX_H_

#include <stdio.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"

typedef struct {
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    int end_loop;
} CAN_t;

int CAN_open(CAN_t *can_connection, const char* can_device);
int CAN_send_frame_str(CAN_t *can_connection, char* frame_data);
int CAN_send_frame(CAN_t *can_connection, struct can_frame *frame);
int CAN_read_loop(CAN_t *can_connection, void (*frame_read_cb)(struct can_frame *frame));
int CAN_end_loop(CAN_t *can_connection);
int CAN_close(CAN_t *can_connection);

#endif
