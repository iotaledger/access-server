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
 * PEP plugin for relay.
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 12.07.2020. Initial version.
 ****************************************************************************/

#include "pep_plugin_relay.h"

#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"
#include "dlog.h"
#include "relay_interface.h"
#include "time_manager.h"

#define RES_BUFF_LEN 80
#define MAX_ACTIONS 10
#define ACTION_NAME_SIZE 16
#define POLICY_ID_SIZE 64
#define ADDR_SIZE 128

typedef int (*action_t)(pdp_action_t* action, int should_log);

typedef struct {
  char action_names[MAX_ACTIONS][ACTION_NAME_SIZE];
  action_t actions[MAX_ACTIONS];
  size_t count;
} action_set_t;

static action_set_t g_action_set;

static int relay_on(pdp_action_t *action, int should_log)
{
  relayinterface_on(0);
  return 0;
}

static int relay_off(pdp_action_t *action, int should_log)
{
  relayinterface_off(0);
  return 0;
}

static int relay_toggle(pdp_action_t *action, int should_log)
{
  relayinterface_toggle(0);
  return 0;
}

static int relay_pulse(pdp_action_t *action, int should_log)
{
  relayinterface_pulse(0);
  return 0;
}

static int destroy_cb(plugin_t* plugin, void* data) { free(plugin->callbacks); }

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

int pep_plugin_relay_initializer(plugin_t* plugin, void* options) {
  g_action_set.actions[0] = relay_on;
  g_action_set.actions[1] = relay_off;
  g_action_set.actions[2] = relay_toggle;
  g_action_set.actions[3] = relay_pulse;
  strncpy(g_action_set.action_names[0], "relay_on", ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[1], "relay_off", ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[2], "relay_toggle", ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[3], "relay_pulse", ACTION_NAME_SIZE);
  g_action_set.count = 4;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEPPLUGIN_ACTION_CB] = action_cb;

  return 0;
}