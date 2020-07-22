/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
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
 * \project IOTA Access
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

#include <net/if.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <linux/can/raw.h>

#include "lib.h"

typedef enum {
  CAN_OPEN_NO_ERROR = 0,
  CAN_OPEN_CONNECTION_ERROR = -1,
  CAN_OPEN_SOCKET_ERROR = -2,
  CAN_OPEN_IOCTL_ERROR = -3,
  CAN_OPEN_BIND_ERROR = -4
} can_open_error_e;

typedef enum {
  CAN_SEND_NO_ERROR = 0,
  CAN_SEND_FRAME_DATA_ERROR = -1,
  CAN_SEND_WRITE_ERROR = -2
} can_send_frame_error_e;

typedef enum {
  CAN_READ_NO_ERROR = 0,
  CAN_READ_CONNECTION_ERROR = -1,
  CAN_READ_RECEIVE_ERROR = -2,
  CAN_READ_INCOMPLETE_ERROR = -3
} can_read_error_e;

typedef enum { CAN_CLOSE_NO_ERROR = 0, CAN_CLOSE_ERROR = -1 } can_close_error_e;

typedef struct {
  int sock;
  struct sockaddr_can addr;
  struct ifreq ifr;
  int end_loop;
} can_t;

int can_open(can_t *can_connection, const char *can_device);
int can_send_frame_str(can_t *can_connection, char *frame_data);
int can_send_frame(can_t *can_connection, struct can_frame *frame);
int can_read_loop(can_t *can_connection, void (*frame_read_cb)(struct can_frame *frame));
int can_end_loop(can_t *can_connection);
int can_close(can_t *can_connection);

#endif
