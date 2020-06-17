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

#include "access.h"

#include <stdlib.h>
#include <string.h>

#include "config_manager.h"
#include "datadumper.h"
#include "pep.h"
#include "policy_loader.h"

#include "can_receiver.h"
#include "canopen_receiver.h"
#include "demo_resolver_can01.h"
#include "demo_resolver_can02.h"
#include "demo_resolver_wallet.h"
#include "gps_receiver.h"
#include "modbus_receiver.h"
#include "obdii_receiver.h"
#include "protocol.h"
#include "resolver.h"
#include "storage.h"
#include "timer.h"
#include "transaction.h"

#define MAX_CLIENT_NAME 32
#define CONFIG_CLIENT_CAN01 "can01"
#define CONFIG_CLIENT_CANOPEN01 "canopen01"
#define CONFIG_CLIENT_OBDII "obdii"

typedef struct {
  int using_can;
  int using_gps;
  int using_obdii;
  int using_car_gw;
  int using_canopen;
  int using_modbus;
  char client_name[MAX_CLIENT_NAME];
  pthread_mutex_t *json_mutex;
  dataset_state_t ddstate;
} access_ctx_t_;

void access_init(access_ctx_t *access_context, wallet_ctx_t *device_wallet, resolver_plugin_initializer_t resolver_init_fn) {
  if (device_wallet == NULL) {
    access_context = NULL;
    return;
  }
  access_ctx_t_ *ctx = calloc(1, sizeof(access_ctx_t_));

  configmanager_init("config.ini");
  configmanager_get_option_string("config", "client", ctx->client_name, MAX_CLIENT_NAME);

  // Register plugins
  timer_init();
  storage_init();

  pep_init();
  protocol_init(device_wallet);
  transaction_init(device_wallet);

  ctx->json_mutex = datadumper_get_mutex();

  pep_register_callback(resolver_init(resolver_init_fn, &ctx->ddstate, (void*)device_wallet));

  if (strncmp(ctx->client_name, CONFIG_CLIENT_CAN01, strlen(CONFIG_CLIENT_CAN01)) == 0) {
#ifdef TINY_EMBEDDED
    ctx->ddstate.options = &VehicleDatasetDemo01_options[0];
    ctx->ddstate.dataset = malloc(sizeof(can01_vehicle_dataset_t));
    dataset_init(&ctx->ddstate);
    canreceiver_init(ctx->ddstate.dataset, json_mutex);
    // GpsReceiver_init(json_mutex);
    ctx->using_can = 1;
    // ctx->using_gps = 1;
#else
    canreceiver_pre_init_setup();
#endif
  } else if (strncmp(ctx->client_name, CONFIG_CLIENT_CANOPEN01, strlen(CONFIG_CLIENT_CANOPEN01)) == 0) {
    resolver_init(demowalletplugin_initializer, &ctx->ddstate, (void *)device_wallet);
#ifdef TINY_EMBEDDED
    ctx->ddstate.options = &VehicleDatasetDemo02_options[0];
    ctx->ddstate.dataset = malloc(sizeof(canopen01_vehicle_dataset_t));
    dataset_init(&ctx->ddstate);
    canopenreceiver_init(ctx->ddstate.dataset, ctx->json_mutex);
    // ModbusReceiver_init(ctx->ddstate.dataset, ctx->json_mutex);
    ctx->using_canopen = 1;
    // ctx->using_modbus = 1;
#else
    canopenreceiver_pre_init_setup();
#endif
  } else if (strncmp(ctx->client_name, CONFIG_CLIENT_OBDII, strlen(CONFIG_CLIENT_OBDII)) == 0) {
    obdiireceiver_init("can0", ctx->json_mutex);
    ctx->using_obdii = 1;
  }

  *access_context = (access_ctx_t)ctx;
  datadumper_init();
}

void access_start(access_ctx_t access_context) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;

  policyloader_start();

  if (ctx->using_modbus == 1) modbusreceiver_start();
  if (ctx->using_can == 1) canreceiver_start();
  if (ctx->using_gps == 1) gpsreceiver_start();
  if (ctx->using_canopen == 1) canopenreceiver_start();
}

void access_deinit(access_ctx_t access_context) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;

  if (ctx->using_canopen == 1) canopenreceiver_deinit();
  if (ctx->using_gps == 1) gpsreceiver_end();
  if (ctx->using_can == 1) canreceiver_deinit();
  if (ctx->using_modbus == 1) modbusreceiver_stop();

  policyloader_stop();

  datadumper_deinit();
  if (ctx->ddstate.dataset != 0) {
    dataset_deinit(&ctx->ddstate);
  }

  timer_deinit();
}

void access_get_ddstate(access_ctx_t access_context, dataset_state_t **ddstate) {
  access_ctx_t_ *ctx = (access_ctx_t_ *)access_context;
  *ddstate = &ctx->ddstate;
}
