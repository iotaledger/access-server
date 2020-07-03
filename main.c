/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "access.h"
#include "can_receiver.h"
#include "canopen_receiver.h"
#include "config_manager.h"
#include "datadumper.h"
#include "dataset.h"
#include "globals_declarations.h"
#include "network.h"
#include "obdii_receiver.h"
#include "pepplugin_can_demo.h"
#include "pepplugin_canopen_demo.h"
#include "pepplugin_wallet_demo.h"
#include "pip_plugin_wallet.h"
#include "storage.h"
#include "timer.h"

#define NODE_URL "nodes.comnet.thetangle.org"
#define NODE_PORT 443
#define NODE_DEPTH 3
#define NODE_MWM 14
#define WALLET_SEED "DEJUXV9ZQMIEXTWJJHJPLAWMOEKGAYDNALKSMCLG9APR9LCKHMLNZVCRFNFEPMGOBOYYIKJNYWSAKVPAI"

int g_task_sleep_time;

#define dlog_printf printf

#define MAX_CLIENT_NAME 32
#define CONFIG_CLIENT_CAN01 "can01"
#define CONFIG_CLIENT_CANOPEN01 "canopen01"
#define CONFIG_CLIENT_OBDII "obdii"

static int using_can;
static int using_gps;
static int using_obdii;
static int using_car_gw;
static int using_canopen;
static int using_modbus;
static char client_name[MAX_CLIENT_NAME];

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static network_ctx_t network_context = 0;
static access_ctx_t access_context = 0;

int main(int argc, char **argv) {
  dataset_state_t ddstate;

  signal(SIGINT, signal_handler);
  sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

  configmanager_init("config.ini");
  configmanager_get_option_string("config", "client", client_name, MAX_CLIENT_NAME);

  int status = configmanager_get_option_int("config", "thread_sleep_period", &g_task_sleep_time);
  if (status != CONFIG_MANAGER_OK) g_task_sleep_time = 1000;  // 1 second

  access_init(&access_context);

  // register plugins
  datadumper_init();
  plugin_t plugin;

  plugin_init(&plugin, peppluginwalletdemo_initializer, NULL);
  access_register_pep_plugin(access_context, &plugin);

  plugin_init(&plugin, pippluginwallet_initializer, NULL);
  access_register_pip_plugin(access_context, &plugin);

  plugin_init(&plugin, pappluginrpi_initializer, NULL);
  access_register_pap_plugin(access_context, &plugin);

  if (strncmp(client_name, CONFIG_CLIENT_CAN01, strlen(CONFIG_CLIENT_CAN01)) == 0) {
    plugin_init(&plugin, pipplugin_canreceiver_initializer, NULL);
    access_register_pip_plugin(access_context, &plugin);
    plugin_init(&plugin, pepplugincandemo_initializer, NULL);
    access_register_pep_plugin(access_context, &plugin);
    using_can = 1;
  } else if (strncmp(client_name, CONFIG_CLIENT_CANOPEN01, strlen(CONFIG_CLIENT_CANOPEN01)) == 0) {
    plugin_init(&plugin, pipplugin_canopenreceiver_initializer, NULL);
    access_register_pip_plugin(access_context, &plugin);
    using_canopen = 1;
  } else if (strncmp(client_name, CONFIG_CLIENT_OBDII, strlen(CONFIG_CLIENT_OBDII)) == 0) {
    plugin_init(&plugin, pipplugin_obdiireceiver_initializer, NULL);
    access_register_pip_plugin(access_context, &plugin);
    using_obdii = 1;
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

  datadumper_deinit();
  return 0;
}
