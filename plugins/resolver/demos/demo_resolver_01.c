/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \file demo_01_plugin.c
 * \brief
 * Resolver plugin for CAN demo. There are two variants, one is using relay
 * board directly connected to rpi3, and other is using relay board
 * connected through TCP socket.
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/

#include "demo_resolver_01.h"

#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "relay_interface.h"

#include "can_receiver.h"
#include "json_interface.h"
#include "config_manager.h"
#include "transaction.h"

#define ADDR_SIZE 128

static char relayboard_addr[ADDR_SIZE] = "127.0.0.1";

void Demo01Plugin_set_relayboard_addr(const char* addr)
{
    strncpy(relayboard_addr, addr, sizeof(relayboard_addr));
}

static int demo01_car_lock(resolver_action_data_t *action, int should_log)
{
    RelayInterface_pulse(0);
    return 0;
}

static int demo01_car_unlock(resolver_action_data_t *action, int should_log)
{
    RelayInterface_pulse(1);
    return 0;
}

static int demo01_start_engine(resolver_action_data_t *action, int should_log)
{
    RelayInterface_pulse(2);
    return 0;
}

static int demo01_open_trunk(resolver_action_data_t *action, int should_log)
{
    RelayInterface_pulse(3);
    return 0;
}

static void socket_send_byte_to_port(int portname)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char data = '1';

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed... Not sending\n");
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
        sizeof(timeout)) < 0)
    {
        perror("Resolved setsockopt failed");
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(relayboard_addr);
    servaddr.sin_port = htons(portname);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        perror("Resolver connect failed");
    }
    else
    {
        write(sockfd, &data, 1);
    }

    close(sockfd);
}

static int demo01_tcp_car_lock(resolver_action_data_t *action, int should_log)
{
    socket_send_byte_to_port(2222);
    return 0;
}

static int demo01_tcp_car_unlock(resolver_action_data_t *action, int should_log)
{
    socket_send_byte_to_port(2223);
    return 0;
}

static int demo01_tcp_start_engine(resolver_action_data_t *action, int should_log)
{
    socket_send_byte_to_port(2224);
    return 0;
}

static int demo01_tcp_open_trunk(resolver_action_data_t *action, int should_log)
{
    socket_send_byte_to_port(2225);
    return 0;
}

static resolver_plugin_t* g_action_set = NULL;

static void init_ds_interface(Dataset_state_t* vdstate)
{
#ifdef TINY_EMBEDDED
    CanReceiver_deinit();
    // Re-init receiver with new dataset
#endif
    
    vdstate->options = &VehicleDatasetDemo01_options[0];
    Dataset_init(vdstate);
    CanReceiver_init(vdstate->dataset, JSONInterface_get_mutex());
}

static void init_ds_interface_tcp(Dataset_state_t* vdstate)
{
#ifdef TINY_EMBEDDED
    CanReceiver_deinit();
    // Re-init receiver with new dataset
#endif

    vdstate->options = &VehicleDatasetDemo01_options[0];
    Dataset_init(vdstate);
    CanReceiver_init(vdstate->dataset, JSONInterface_get_mutex());
}

static void start_ds_interface()
{
    CanReceiver_start();
}

static void stop_ds_interface()
{
    CanReceiver_deinit();
}

static void term_ds_interface(Dataset_state_t* vdstate)
{
    Demo01Plugin_terminizer();
    vdstate->options = NULL;
    Dataset_deinit(vdstate);
}

void Demo01Plugin_initializer(resolver_plugin_t* action_set, void* options)
{
    int cfg_status = ConfigManager_get_option_string("demo01plugin", "relayboard_address", relayboard_addr, ADDR_SIZE);

    if (g_action_set == NULL && action_set == NULL && options == NULL)
    {
        return;
    }

    if (action_set != NULL)
    {
        g_action_set = action_set;
    }
    g_action_set->actions[0] = demo01_car_unlock;
    g_action_set->actions[1] = demo01_car_lock;
    g_action_set->actions[2] = demo01_open_trunk;
    g_action_set->actions[3] = demo01_start_engine;
    strncpy(g_action_set->action_names[0], "open_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[1], "close_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[2], "open_trunk", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[3], "start_engine", RES_ACTION_NAME_SIZE);
    g_action_set->count = 4;
    g_action_set->init_ds_interface_cb = init_ds_interface;
    g_action_set->stop_ds_interface_cb = stop_ds_interface;
    g_action_set->start_ds_interface_cb = start_ds_interface;
    g_action_set->term_ds_interface_cb = term_ds_interface;
}

void Demo01Plugin_initializer_tcp(resolver_plugin_t* action_set, void* options)
{
    if (g_action_set == NULL && action_set == NULL && options == NULL)
    {
        return;
    }

    if (action_set != NULL)
    {
        g_action_set = action_set;
    }
    g_action_set->actions[0] = demo01_tcp_car_unlock;
    g_action_set->actions[1] = demo01_tcp_car_lock;
    g_action_set->actions[2] = demo01_tcp_open_trunk;
    g_action_set->actions[3] = demo01_tcp_start_engine;
    strncpy(g_action_set->action_names[0], "open_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[1], "close_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[2], "open_trunk", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[3], "start_engine", RES_ACTION_NAME_SIZE);
    g_action_set->count = 4;
    g_action_set->init_ds_interface_cb = init_ds_interface_tcp;
    g_action_set->stop_ds_interface_cb = stop_ds_interface;
    g_action_set->start_ds_interface_cb = start_ds_interface;
    g_action_set->term_ds_interface_cb = term_ds_interface;
}

void Demo01Plugin_terminizer()
{
    g_action_set = NULL;
}
