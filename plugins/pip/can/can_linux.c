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
 * \project IOTA Access
 * \file can_linux.c
 * \brief
 * Implementation of interface for linux CAN
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#include <string.h>
#include <unistd.h>

#include "can_linux.h"
#include "globals_declarations.h"

#define CAN_DEVICE_NAME_LEN 5
#define CAN_CTRLMSG_LEN 1024
#define CAN_TIMEOUT_uSEC 30000

int can_open(can_t *can_connection, const char *can_device) {
  if (can_connection == NULL) return CAN_OPEN_CONNECTION_ERROR;

  if ((can_connection->sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("socket");
    return CAN_OPEN_SOCKET_ERROR;
  }

  can_connection->addr.can_family = AF_CAN;

  memset(&can_connection->ifr.ifr_name, 0, sizeof(can_connection->ifr.ifr_name));
  if (can_device == NULL) {
    strncpy(can_connection->ifr.ifr_name, "can0", strlen("can0"));
  } else {
    strncpy(can_connection->ifr.ifr_name, can_device, CAN_DEVICE_NAME_LEN);
  }

  if (ioctl(can_connection->sock, SIOCGIFINDEX, &can_connection->ifr) < 0) {
    perror("SIOCGIFINDEX");
    return CAN_OPEN_IOCTL_ERROR;
  }
  can_connection->addr.can_ifindex = can_connection->ifr.ifr_ifindex;

  if (bind(can_connection->sock, (struct sockaddr *)&can_connection->addr, sizeof(can_connection->addr)) < 0) {
    perror("bind");
    return CAN_OPEN_BIND_ERROR;
  }

  can_connection->end_loop = 0;

  return CAN_OPEN_NO_ERROR;
}

int can_send_frame_str(can_t *can_connection, char *frame_data) {
  struct can_frame frame;

  if (frame_data == NULL) return CAN_SEND_FRAME_DATA_ERROR;

  /* parse CAN frame */
  if (parse_canframe(frame_data, &frame)) {
    fprintf(stderr, "\nWrong CAN-frame format!\n\n");
    fprintf(stderr, "Try: <can_id>#{R|data}\n");
    fprintf(stderr, "can_id can have 3 (SFF) or 8 (EFF) hex chars\n");
    fprintf(stderr, "data has 0 to 8 hex-values that can (optionally)");
    fprintf(stderr, " be seperated by '.'\n\n");
    fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / ");
    fprintf(stderr, "5AA# /\n     1F334455#1122334455667788 / 123#R ");
    fprintf(stderr, "for remote transmission request.\n\n");
    return CAN_SEND_FRAME_DATA_ERROR;
  }

  if ((write(can_connection->sock, &frame, sizeof(frame))) != sizeof(frame)) {
    perror("write");
    return CAN_SEND_WRITE_ERROR;
  }

  return CAN_SEND_NO_ERROR;
}

int can_send_frame(can_t *can_connection, struct can_frame *frame) {
  if (can_connection == NULL || frame == NULL) return CAN_SEND_FRAME_DATA_ERROR;

  if ((write(can_connection->sock, frame, sizeof(struct can_frame))) != sizeof(struct can_frame)) {
    perror("write");
    return CAN_SEND_WRITE_ERROR;
  }

  return CAN_SEND_NO_ERROR;
}

int can_end_loop(can_t *can_connection) { can_connection->end_loop = 1; }

int can_read_loop(can_t *can_connection, void (*frame_read_cb)(struct can_frame *frame)) {
  fd_set rdfs;
  struct iovec iov;
  struct can_frame frame;
  struct msghdr msg;
  char ctrlmsg[CAN_CTRLMSG_LEN];
  int ret = CAN_READ_NO_ERROR;
  int nbytes;

  if (can_connection == NULL) return CAN_READ_CONNECTION_ERROR;

  iov.iov_base = &frame;
  msg.msg_name = &can_connection->addr;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = &ctrlmsg;

  struct timeval tv_timeout;
  tv_timeout.tv_sec = 0;
  tv_timeout.tv_usec = CAN_TIMEOUT_uSEC;

  while (can_connection->end_loop != 1) {
    FD_ZERO(&rdfs);
    FD_SET(can_connection->sock, &rdfs);

    if ((ret = select(can_connection->sock + 1, &rdfs, NULL, NULL, &tv_timeout)) < 0) {
      break;
    }
    tv_timeout.tv_sec = 0;
    tv_timeout.tv_usec = CAN_TIMEOUT_uSEC;

    if (FD_ISSET(can_connection->sock, &rdfs)) {
      iov.iov_len = sizeof(frame);
      msg.msg_namelen = sizeof(can_connection->addr);
      msg.msg_controllen = sizeof(ctrlmsg);
      msg.msg_flags = 0;

      nbytes = recvmsg(can_connection->sock, &msg, 0);
      if (nbytes < 0) {
        perror("read");
        return CAN_READ_RECEIVE_ERROR;
      }

      if (nbytes < sizeof(struct can_frame)) {
        fprintf(stderr, "read: incomplete CAN frame\n");
        return CAN_READ_INCOMPLETE_ERROR;
      }

      if (frame_read_cb != NULL) {
        frame_read_cb(&frame);
      }
    } else {
      usleep(g_task_sleep_time);
    }

    fflush(stdout);
  }

  return ret;
}

int can_close(can_t *can_connection) {
  if (can_connection == NULL) return CAN_CLOSE_ERROR;
  close(can_connection->sock);
  return CAN_CLOSE_NO_ERROR;
}
