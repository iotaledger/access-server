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
 * \file demo_02_plugin.c
 * \brief
 * Resolver plugin for CANOpen demo using relay board connected directly to
 * rpi3.
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/

#include "demo_resolver_02.h"

#include <string.h>

#include "relay_interface.h"

#include "canopen_receiver.h"
#include "json_interface.h"

static int demo02_vehicle_lock(int should_log)
{
    RelayInterface_off(3);
    return 0;
}

static int demo02_vehicle_unlock(int should_log)
{
    RelayInterface_on(3);
    return 0;
}

static int demo02_honk(int should_log)
{
    RelayInterface_pulse(2);
    return 0;
}

static int demo02_alarm_on(int should_log)
{
    RelayInterface_on(1);
    return 0;
}

static int demo02_alarm_off(int should_log)
{
    RelayInterface_off(1);
    return 0;
}

static resolver_plugin_t* g_action_set = NULL;

static void init_cb(Dataset_state_t* vdstate)
{
    char canopen_port_name[RES_MAX_STR_SIZE];
    int canopen_node_id;

    CanopenReceiver_getPortName(canopen_port_name, RES_MAX_STR_SIZE);
    canopen_node_id = CanopenReceiver_getNodeId();
#ifdef TINY_EMBEDDED
    CanopenReceiver_deinit();
    // Re-init receiver with new dataset
#endif

    Demo02Plugin_initializer(NULL);
    vdstate->options = &VehicleDatasetDemo02_options[0];
    Dataset_init(vdstate);
    CanopenReceiver_init(vdstate->dataset, JSONInterface_get_mutex(), canopen_port_name, canopen_node_id);
}

static void start_cb() {}
static void stop_cb() {}

static void term_cb(Dataset_state_t* vdstate)
{
    Demo02Plugin_terminizer();
    vdstate->options = NULL;
    Dataset_deinit(vdstate);
    CanopenReceiver_deinit();
}

void Demo02Plugin_initializer(resolver_plugin_t* action_set)
{
    if (action_set == NULL && g_action_set == NULL)
    {
        return;
    }

    if (action_set != NULL)
    {
        g_action_set = action_set;
    }

    g_action_set->actions[0] = demo02_vehicle_unlock;
    g_action_set->actions[1] = demo02_vehicle_lock;
    g_action_set->actions[2] = demo02_honk;
    g_action_set->actions[3] = demo02_alarm_on;
    g_action_set->actions[4] = demo02_alarm_off;
    strncpy(g_action_set->action_names[0], "open_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[1], "close_door", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[2], "honk", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[3], "alarm_on", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[4], "alarm_off", RES_ACTION_NAME_SIZE);
    g_action_set->count = 5;
    g_action_set->init_ds_interface_cb = init_cb;
    g_action_set->start_ds_interface_cb = start_cb;
    g_action_set->stop_ds_interface_cb = stop_cb;
    g_action_set->term_ds_interface_cb = term_cb;
}

void Demo02Plugin_terminizer()
{
    g_action_set = NULL;
}
