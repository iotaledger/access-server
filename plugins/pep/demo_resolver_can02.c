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

#include "datadumper.h"
#include "dlog.h"
#include "pep_plugin.h"
#include "relay_interface.h"
#include "time_manager.h"

#define RES_MAX_RESOLVER_ACTIONS 10
#define RES_ACTION_NAME_SIZE 16
#define RES_POL_ID_STR_LEN 64
#define RES_BUFF_LEN 80

static int demo02_vehicle_lock(pdp_action_t* action, int should_log) {
  relayinterface_off(3);
  return 0;
}

static int demo02_vehicle_unlock(pdp_action_t* action, int should_log) {
  relayinterface_on(3);
  return 0;
}

static int demo02_honk(pdp_action_t* action, int should_log) {
  relayinterface_pulse(2);
  return 0;
}

static int demo02_alarm_on(pdp_action_t* action, int should_log) {
  relayinterface_on(1);
  return 0;
}

static int demo02_alarm_off(pdp_action_t* action, int should_log) {
  relayinterface_off(1);
  return 0;
}

typedef int (*action_t)(pdp_action_t* action, int should_log);

typedef struct {
  char action_names[RES_MAX_RESOLVER_ACTIONS][RES_ACTION_NAME_SIZE];
  action_t actions[RES_MAX_RESOLVER_ACTIONS];
  size_t count;
} action_set_t;

action_set_t g_action_set;

static int destroy_cb(plugin_t* plugin, void* data) {
  free(plugin->callbacks);
  return 0;
}

static int action_cb(plugin_t* plugin, void* data) {
  pepplugin_args_t* args = (pepplugin_args_t*)data;
  pdp_action_t* action = &args->action;
  char* obligation = args->obligation;
  bool should_log = FALSE;
  char buf[RES_BUFF_LEN];
  int status = 0;

  // handle obligations
  if (0 == memcmp(obligation, "log_event", strlen("log_event"))) {
    should_log = TRUE;
  }

  // execute action
  for (int i = 0; i < g_action_set.count; i++) {
    if (memcmp(action->value, g_action_set.action_names[i], strlen(g_action_set.action_names[i])) == 0) {
      timemanager_get_time_string(buf, RES_BUFF_LEN);
      dlog_printf("%s %s\t<Action performed>\n", buf, action->value);
      status = g_action_set.actions[i](action, should_log);
      break;
    }
  }
  return status;
}

int demo02plugin_initializer(plugin_t* plugin, void* options) {
  if (plugin == NULL) {
    return -1;
  }

  g_action_set.actions[0] = demo02_vehicle_unlock;
  g_action_set.actions[1] = demo02_vehicle_lock;
  g_action_set.actions[2] = demo02_honk;
  g_action_set.actions[3] = demo02_alarm_on;
  g_action_set.actions[4] = demo02_alarm_off;
  strncpy(g_action_set.action_names[0], "open_door", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[1], "close_door", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[2], "honk", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[3], "alarm_on", RES_ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[4], "alarm_off", RES_ACTION_NAME_SIZE);
  g_action_set.count = 5;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEPPLUGIN_ACTION_CB] = action_cb;

  return 0;
}
