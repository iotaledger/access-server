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
