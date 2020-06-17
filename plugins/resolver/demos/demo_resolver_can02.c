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
 * \file demo_resolver_can02.c
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

#include "demo_resolver_can02.h"

#include <string.h>

#include "relay_interface.h"

#include "canopen_receiver.h"
#include "datadumper.h"

static int demo02_vehicle_lock(resolver_action_data_t* action, int should_log) {
  relayinterface_off(3);
  return 0;
}

static int demo02_vehicle_unlock(resolver_action_data_t* action, int should_log) {
  relayinterface_on(3);
  return 0;
}

static int demo02_honk(resolver_action_data_t* action, int should_log) {
  relayinterface_pulse(2);
  return 0;
}

static int demo02_alarm_on(resolver_action_data_t* action, int should_log) {
  relayinterface_on(1);
  return 0;
}

static int demo02_alarm_off(resolver_action_data_t* action, int should_log) {
  relayinterface_off(1);
  return 0;
}

static resolver_plugin_t* g_action_set = NULL;

static void init_cb(dataset_state_t* vdstate) {
#ifdef TINY_EMBEDDED
  canopenreceiver_deinit();
  // Re-init receiver with new dataset
#endif

  vdstate->options = &vehicledatasetdemo02_options[0];
  dataset_init(vdstate);
  canopenreceiver_init(vdstate->dataset, datadumper_get_mutex());
}

static void start_cb() {}
static void stop_cb() {}

static void term_cb(dataset_state_t* vdstate) {
  demo02plugin_terminizer();
  vdstate->options = NULL;
  dataset_deinit(vdstate);
  canopenreceiver_deinit();
}

void demo02plugin_initializer(resolver_plugin_t* action_set, void* options) {
  if (action_set == NULL && g_action_set == NULL) {
    return;
  }

  if (action_set != NULL) {
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

void demo02plugin_terminizer() { g_action_set = NULL; }
