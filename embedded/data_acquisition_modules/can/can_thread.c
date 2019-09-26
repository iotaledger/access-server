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
 * \file can_thread.c
 * \brief
 * CAN reader thread implementation
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#include "can_thread.h"
#include "can_linux.h"

#include <string.h>
#include <pthread.h>

void CanThread_init(CanThread_instance_t* inst, const char* can_bus_name, void (*can_cb)(struct can_frame *frame))
{
    strncpy(inst->can_bus_name, can_bus_name, 5);
    inst->can_frame_read_cb = can_cb;
}

static void *can_thread(void *ptr)
{
    CanThread_instance_t *targs = (CanThread_instance_t*)ptr;
    CAN_open(&targs->can_connection, targs->can_bus_name);
    CAN_read_loop(&targs->can_connection, targs->can_frame_read_cb);
    CAN_close(&targs->can_connection);
}

int CanThread_start(CanThread_instance_t* inst)
{
    if (pthread_create(&inst->thread, NULL, can_thread, inst)){
        fprintf(stderr, "Error creating CAN reader thread\n");
        return 1;
    }
    return 0;
}

int CanThread_stop(CanThread_instance_t* inst)
{
    CAN_end_loop(&inst->can_connection);
    if (pthread_join(inst->thread, NULL)){
        fprintf(stderr, "Error joining can reader thread\n");
        return 1;
    }
    return 0;
}


