/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
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
 * \project Decentralized Access Control
 * \file resolver.c
 * \brief
 * Implementation of Resolver
 *
 * @Author Vladimir Vojnovic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 03.10.2018. Initial version.
 * 28.02.2020. Added data sharing through action functionality
 * 14.05.2020. Refactoring
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "resolver.h"
#include <string.h>
#include "datadumper.h"
#include "dlog.h"
#include "time_manager.h"
#include "timer.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define RES_DATASET_NUM_SIZE 2
#define RES_DATASET_NUM_POSITION 9
#define RES_BUFF_LEN 80

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
//@TODO: This can be removed by adding state and timer to resolver_plugin_t, which will be passed as functions argument
static char action_s[] = "<Action performed>";
static int timerId = -1;
static dataset_state_t *g_dstate = NULL;
static resolver_plugin_t resolver_action_set = {0};

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static int start_data_sharing(const char *action, unsigned long end_time);
static int stop_data_sharing();

static void timer_handler(size_t timer_id, void *user_data) { stop_data_sharing(); }

static int start_data_sharing(const char *action, unsigned long end_time) {
  if (action == NULL) {
    dlog_printf("\nERROR[%s] - Wrong input parameter\n", __FUNCTION__);
    return -1;
  }

  // Initialize interface
  resolver_action_set.init_ds_interface_cb(g_dstate);

  // Chose dataset
  char *dataset;
  char dataset_num[RES_DATASET_NUM_SIZE];

  memcpy(dataset_num, &action[RES_DATASET_NUM_POSITION], RES_DATASET_NUM_SIZE);

  dataset = dataset_options[atoi(dataset_num) - 1];

  // Tokenize dataset
  char *tok = NULL;

  tok = strtok(dataset, "|");

  while (tok != NULL) {
    for (int i = 0; i < g_dstate->options_count; i++) {
      // Set options that needs to ne acquried
      if (strncmp(g_dstate->tokens[i].name, tok,
                  strlen(tok) < strlen(g_dstate->tokens[i].name) ? strlen(tok) : strlen(g_dstate->tokens[i].name)) ==
          0) {
        g_dstate->tokens[i].val = 1;
      }
    }

    tok = strtok(NULL, "|");
  }

  unsigned char *dataset_uint8 = (unsigned char *)g_dstate->dataset;

  for (int i = 0; i < g_dstate->options_count; i++) {
    dataset_uint8[i] = g_dstate->tokens[i].val;
  }

  timerId = timer_start(end_time - timemanager_get_time_epoch(), (time_handler)timer_handler, TIMER_SINGLE_SHOT, NULL);
  resolver_action_set.start_ds_interface_cb();

  return 0;
}

static int stop_data_sharing() {
  timer_stop(timerId);
  resolver_action_set.stop_ds_interface_cb();

  return 0;
}

static int action_resolve(resolver_action_data_t *action, int should_log) {
  char buf[RES_BUFF_LEN];
  int retval = -1;

  if (0 == memcmp(action->value, "start_ds_", strlen("start_ds_") - 1)) {
    retval = start_data_sharing(action->value, action->stop_time);
  } else if (0 == memcmp(action->value, "stop_ds", strlen("stop_ds"))) {
    retval = stop_data_sharing();
  } else {
    for (int i = 0; i < resolver_action_set.count; i++) {
      if (memcmp(action->value, resolver_action_set.action_names[i], strlen(resolver_action_set.action_names[i])) ==
          0) {
        timemanager_get_time_string(buf, RES_BUFF_LEN);
        dlog_printf("%s %s\t%s\n", buf, action, action_s);
        retval = resolver_action_set.actions[i](action, should_log);
        break;
      }
    }
  }
  return retval;
}

static bool resolver_callback(char *obligation, void *action) {
  bool should_log = FALSE;
  resolver_action_data_t *action_r;

  if (action == NULL) {
    dlog_printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  } else {
    action_r = (resolver_action_data_t *)action;
  }

  // TODO: only "log_event" obligation is supported currently
  if (0 == memcmp(obligation, "log_event", strlen("log_event"))) {
    should_log = TRUE;
  }

  // TODO: better handling of end_time parameter
  if (action_resolve(action_r, should_log) == -1) {
    dlog_printf("\nERROR[%s]: Resolving action failed.\n", __FUNCTION__);
    return FALSE;
  }

  return TRUE;
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
resolver_fn resolver_init(resolver_plugin_initializer_t initializer, dataset_state_t *dstate) {
  initializer(&resolver_action_set);
  g_dstate = dstate;

  return resolver_callback;
}

void resolver_term(resolver_plugin_terminizer_t terminizer) {
  terminizer(&resolver_action_set);
  g_dstate = NULL;

  pep_unregister_callback();
}
