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

#include "can_msgs.h"

const int supported_body_msgs[] = {
    0x40,
    0xB0,
    0xE0,
    0x100,
    0x1B0,
    0x250,
    0x270,
    0x310,
    0x1D0,
    0x360,
    0x380,
    0x330,
    0x259,
    0x95,
    0x1E0,
    0x130,
    0x160
};

const int supported_chas_msgs[] = {
    0xD0,
    0xE0,
    0xF0,
    0x3A0,
    0x1C0,
    0x1F0,
    0x240,
    0x2E0,
    0x380,
    0x1A,
    0x15,
    0x35,
    0x100,
    0x110,
    0x130,
    0x210,
    0x23A
};

int CanMsgs_body_msg_supported(unsigned int msg_id)
{
    int supported = 0;
    for (int i=0; i<sizeof(supported_body_msgs) / sizeof(int); i++){
        if (supported_body_msgs[i] == msg_id) {
            supported = 1;
            break;
        }
    }
    return supported;
}

int CanMsgs_chas_msg_supported(unsigned int msg_id)
{
    int supported = 0;
    for (int i=0; i<sizeof(supported_chas_msgs) / sizeof(int); i++){
        if (supported_chas_msgs[i] == msg_id) {
            supported = 1;
            break;
        }
    }
    return supported;
}
