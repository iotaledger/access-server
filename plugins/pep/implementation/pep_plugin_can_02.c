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
 * \file pep_plugin_can_02.c
 * \brief
 * pep_plugin plugin for CANOpen demo using relay board connected directly to
 * rpi3.
 *
 * @Author Djordje Golubovic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 * 19.06.2020. Refactoring
 ****************************************************************************/

#include "pep_plugin_can_02.h"

#include <string.h>

#include "relay_interface.h"

#include "canopen_receiver.h"
#include "datadumper.h"

static int can_02_vehicle_lock(pep_plugin_action_data_t* action, int should_log) {
  relayinterface_off(3);
  return 0;
}

static int can_02_vehicle_unlock(pep_plugin_action_data_t* action, int should_log) {
  relayinterface_on(3);
  return 0;
}

static int can_02_honk(pep_plugin_action_data_t* action, int should_log) {
  relayinterface_pulse(2);
  return 0;
}

static int can_02_alarm_on(pep_plugin_action_data_t* action, int should_log) {
  relayinterface_on(1);
  return 0;
}

static int can_02_alarm_off(pep_plugin_action_data_t* action, int should_log) {
  relayinterface_off(1);
  return 0;
}

static pep_plugin_t* g_action_set = NULL;

static void init_cb(dataset_state_t* vdstate) {
#ifdef TINY_EMBEDDED
  canopenreceiver_deinit();
  // Re-init receiver with new dataset
#endif

  vdstate->options = &vehicledataset_can_02_options[0];
  dataset_init(vdstate);
  canopenreceiver_init(vdstate->dataset, datadumper_get_mutex());
}

static void start_cb() {}
static void stop_cb() {}

static void term_cb(dataset_state_t* vdstate) {
  can_02_pep_plugin_terminizer();
  vdstate->options = NULL;
  dataset_deinit(vdstate);
  canopenreceiver_deinit();
}

void can_02_pep_plugin_initializer(pep_plugin_t* action_set, void* options) {
  if (action_set == NULL && g_action_set == NULL) {
    return;
  }

  if (action_set != NULL) {
    g_action_set = action_set;
  }

  g_action_set->actions[0] = can_02_vehicle_unlock;
  g_action_set->actions[1] = can_02_vehicle_lock;
  g_action_set->actions[2] = can_02_honk;
  g_action_set->actions[3] = can_02_alarm_on;
  g_action_set->actions[4] = can_02_alarm_off;
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

void can_02_pep_plugin_terminizer() { g_action_set = NULL; }
