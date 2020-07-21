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

#define MAX_STR_LEN 512
#define SEED_LEN 81+1
#define MAX_PEM_LEN 4 * 1024

int g_task_sleep_time = 1000;

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static network_ctx_t network_context;
static access_ctx_t access_context;
static wallet_ctx_t *wallet_context;

int wallet_init(){
  char node_url[MAX_STR_LEN] = {0};
  char seed[SEED_LEN] = {0};
  uint8_t node_mwm;
  uint16_t port;
  uint32_t node_depth;
  char pem_file[MAX_STR_LEN] = {0};
  char ca_pem[MAX_PEM_LEN] = {0};

  config_manager_get_option_string("wallet", "url", node_url, MAX_STR_LEN);
  config_manager_get_option_string("wallet", "seed", seed, 81 + 1);
  config_manager_get_option_string("wallet", "pem_file_path", pem_file, MAX_STR_LEN);
  config_manager_get_option_int("wallet", "mwm", (int*)&node_mwm);
  config_manager_get_option_int("wallet", "port", (int*)&port);
  config_manager_get_option_int("wallet", "depth", (int*)&node_depth);

  if (strlen(pem_file) == 0) {
    printf("\nERROR[%s]: PEM file for wallet not defined in config.\n", __FUNCTION__);
    return -1;
  }

  FILE* f = fopen(pem_file, "r");
  if (f == NULL) {
    printf("\nERROR[%s]: PEM file (%s) not found.\n", __FUNCTION__, pem_file);
    return -1;
  }
  fread(ca_pem, MAX_PEM_LEN, 1, f);
  fclose(f);

  wallet_context = wallet_create(node_url, port, ca_pem, node_depth, node_mwm, seed);
  if (wallet_context == NULL) {
    printf("\nERROR[%s]: Wallet creation failed.\n", __FUNCTION__);
    return -1;
  }

  return 0;
}

int main(int argc, char **argv) {
  config_manager_init("config.ini");

  signal(SIGINT, signal_handler);
  sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

  access_init(&access_context);
  if (wallet_init() != 0){
    printf("\nERROR[%s]: Wallet creation failed. Aborting.\n", __FUNCTION__);
  }

  // register plugins
  plugin_t plugin;

  if (plugin_init(&plugin, pep_plugin_print_initializer, wallet_context) == 0) {
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

  wallet_destory(&wallet_context);

  return 0;
}
