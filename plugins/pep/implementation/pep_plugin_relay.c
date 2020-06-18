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
 * \file pep_plugin_relay.c
 * \brief
 * pep_plugin plugin for Raspberry Pi Relay Shield
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 16.06.2020. Initial version.
 * 19.06.2020. Refactoring
 ****************************************************************************/

#include "pep_plugin_relay.h"

#include "datadumper.h"
#include "config_manager.h"

static pep_plugin_t* g_action_set = NULL;

static int relay_pep_plugin_on(pep_plugin_action_data_t *action, int should_log)
{
    relayinterface_on(0);
    return 0;
}

static int relay_pep_plugin_off(pep_plugin_action_data_t *action, int should_log)
{
    relayinterface_off(0);
    return 0;
}

static int relay_pep_plugin_toggle(pep_plugin_action_data_t *action, int should_log)
{
    relayinterface_toggle(0);
    return 0;
}

static int relay_pep_plugin_pulse(pep_plugin_action_data_t *action, int should_log)
{
    relayinterface_pulse(0);
    return 0;
}

static void init_ds_interface(dataset_state_t* vdstate)
{

}

static void start_ds_interface()
{

}

static void stop_ds_interface()
{

}

static void term_ds_interface(dataset_state_t* vdstate)
{

}

void relay_pep_plugin_initializer(pep_plugin_t* action_set)
{
    if (action_set == NULL)
    {
        return;
    }

    if (action_set != NULL)
    {
        g_action_set = action_set;
    }

    g_action_set->actions[0] = relay_pep_plugin_on;
    g_action_set->actions[1] = relay_pep_plugin_off;
    g_action_set->actions[2] = relay_pep_plugin_toggle;
    g_action_set->actions[3] = relay_pep_plugin_pulse;
    strncpy(g_action_set->action_names[0], "relay_pep_plugin_on", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[1], "relay_pep_plugin_off", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[2], "relay_pep_plugin_toggle", RES_ACTION_NAME_SIZE);
    strncpy(g_action_set->action_names[3], "relay_pep_plugin_pulse", RES_ACTION_NAME_SIZE);
    g_action_set->count = 4;
    g_action_set->init_ds_interface_cb = init_ds_interface;
    g_action_set->stop_ds_interface_cb = stop_ds_interface;
    g_action_set->start_ds_interface_cb = start_ds_interface;
    g_action_set->term_ds_interface_cb = term_ds_interface;
}

void relay_pep_plugin_terminizer()
{
    g_action_set = NULL;
}