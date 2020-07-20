/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
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

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "access.h"
#include "config_manager.h"
#include "dataset.h"
#include "network.h"
#include "pep_plugin_print.h"
#include "pap_plugin_unix.h"

int g_task_sleep_time = 1000;

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static network_ctx_t network_context = 0;
static access_ctx_t access_context = 0;

int main(int argc, char **argv) {
  dataset_state_t ddstate;

  signal(SIGINT, signal_handler);
  sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

  access_init(&access_context);

  // register plugins
  plugin_t plugin;

  if (plugin_init(&plugin, pep_plugin_print_initializer, NULL) == 0) {
    access_register_pep_plugin(access_context, &plugin);
  }

  if (plugin_init(&plugin, pap_plugin_unix_initializer, NULL) == 0) {
    access_register_pap_plugin(access_context, &plugin);
  }

  // end register plugins

  network_init(&network_context);

  access_start(access_context);
  if (network_start(network_context) != 0) {
    fprintf(stderr, "Error starting Network actor\n");
    running = 0;
  }

  while (running == 1) usleep(g_task_sleep_time);

  // Stop threads
  network_stop(network_context);

  // Deinit modules
  access_deinit(access_context);

  return 0;
}
