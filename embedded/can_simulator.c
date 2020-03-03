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

#include "can_linux.h"
#include "can_msgs.h"
#include "config_manager.h"

#include <signal.h>
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int g_task_sleep_time;
static ConfigManager_config_t config;

static CAN_t can_connection;

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static void be2le(unsigned char *in, unsigned char *out)
{
    unsigned char data[8];
    for (int i=0; i<8; i++){
        data[i] = in[7-i];
    }
    for (int i=0; i<8; i++){
        out[i] = data[i];
    }
}

int main(int argc, char** argv) {
    ConfigManager_load(&config, "config.ini", argc, argv);
    signal(SIGINT, signal_handler);
    g_task_sleep_time = (int)(config.thread_sleep_period * 1000);

    int r = CAN_open(&can_connection, config.symcan_ifname);

    struct can_frame frame_to_be_sent = {0};
    CanMsgs_BodyMessage_t body_msg_payload = {0};
    CanMsgs_ChasMessage_t chas_msg_payload = {0};

    while (running == 1) {
        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        float rand_float;
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 4095.;
        body_msg_payload.msg_0x1D0.AmbTIndcd = (uint32_t)rand_float;
        body_msg_payload.msg_0x1D0.AmbTIndcdWithUnit_UB = 1;
        frame_to_be_sent.can_id = 0x1D0;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 1023.;
        body_msg_payload.msg_0x270.FuLvlIndcdVal = (uint32_t)rand_float;
        body_msg_payload.msg_0x270.FuLvlIndcd_UB = 1;
        frame_to_be_sent.can_id = 0x270;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 7.;
        body_msg_payload.msg_0x100.LockgCenStsForUsrFb = (uint32_t)rand_float;
        body_msg_payload.msg_0x100.LockgCenStsForUsrFb_UB = 1;
        frame_to_be_sent.can_id = 0x100;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 3.;
        body_msg_payload.msg_0xE0.TrSts = (uint32_t)rand_float;
        body_msg_payload.msg_0xE0.TrSts_UB = 1;
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 3.;
        body_msg_payload.msg_0xE0.DoorPassSts = (uint32_t)rand_float;
        body_msg_payload.msg_0xE0.DoorPassSts_UB = 1;
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 3.;
        body_msg_payload.msg_0xE0.DoorPassReSts = (uint32_t)rand_float;
        body_msg_payload.msg_0xE0.DoorPassReSts_UB = 1;
        frame_to_be_sent.can_id = 0xE0;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 3.;
        body_msg_payload.msg_0x40.DoorDrvrSts = (uint32_t)rand_float;
        body_msg_payload.msg_0x40.DoorDrvrSts_UB = 1;
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 3.;
        body_msg_payload.msg_0x40.DoorDrvrReSts = (uint32_t)rand_float;
        body_msg_payload.msg_0x40.DoorDrvrReSts_UB = 1;
        frame_to_be_sent.can_id = 0x40;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&body_msg_payload, 0, sizeof(body_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 32767.;
        body_msg_payload.msg_0x300.EngN = (uint32_t)rand_float;
        body_msg_payload.msg_0x300.EngNSafe_UB = 1;
        frame_to_be_sent.can_id = 0x300;
        frame_to_be_sent.can_dlc = sizeof(body_msg_payload);
        memcpy(&frame_to_be_sent.data, &body_msg_payload, sizeof(body_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&chas_msg_payload, 0, sizeof(chas_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 40000. - 20000.;
        chas_msg_payload.msg_0x130.DrvrPrpsnTqReq = (int16_t)rand_float;
        chas_msg_payload.msg_0x130.DrvrPrpsnTqReq_UB = 1;
        frame_to_be_sent.can_id = 0x130;
        frame_to_be_sent.can_dlc = sizeof(chas_msg_payload);
        memcpy(&frame_to_be_sent.data, &chas_msg_payload, sizeof(chas_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&chas_msg_payload, 0, sizeof(chas_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 255.;
        chas_msg_payload.msg_0x210.CluPedlRat = (uint32_t)rand_float;
        chas_msg_payload.msg_0x210.CluPedlRat_UB = 1;
        frame_to_be_sent.can_id = 0x210;
        frame_to_be_sent.can_dlc = sizeof(chas_msg_payload);
        memcpy(&frame_to_be_sent.data, &chas_msg_payload, sizeof(chas_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        memset(&chas_msg_payload, 0, sizeof(chas_msg_payload));
        rand_float = ((float)rand() / (float)(RAND_MAX)) * 1.;
        chas_msg_payload.msg_0x1C0.BrkPedlPsd = (uint32_t)rand_float;
        chas_msg_payload.msg_0x1C0.BrkPedlPsdSafeGroup_UB = 1;
        frame_to_be_sent.can_id = 0x1C0;
        frame_to_be_sent.can_dlc = sizeof(chas_msg_payload);
        memcpy(&frame_to_be_sent.data, &chas_msg_payload, sizeof(chas_msg_payload));
        be2le(frame_to_be_sent.data, frame_to_be_sent.data);
        CAN_send_frame(&can_connection, &frame_to_be_sent);

        usleep(2000000);
    }

    CAN_close(&can_connection);

    return 0;
}
