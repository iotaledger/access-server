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
#include "pep.h"
#include "json_interface.h"

#include "can_receiver.h"
#include "gps_receiver.h"
#include "canopen_receiver.h"
#include "obdii_receiver.h"
#include "modbus_receiver.h"
#include "storage.h"
#include "protocol.h"
#include "timer.h"

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
    Dataset_state_t vdstate;
} Access_ctx_t_;

void Access_init(Access_ctx_t *access_context, wallet_ctx_t *device_wallet)
{
    if (device_wallet == NULL)
    {
        access_context = NULL;
        return;
    }
    Access_ctx_t_ *ctx = calloc(1, sizeof(Access_ctx_t_));

    ConfigManager_init("config.ini");
    ConfigManager_get_option_string("config", "client", ctx->client_name, MAX_CLIENT_NAME);

    // Register plugins
    Timer_init();
    Storage_init();

    PEP_init();
    PROTOCOL_init(device_wallet);
    TRANSACTION_init(device_wallet);

    ctx->json_mutex = JSONInterface_get_mutex();

    if (strncmp(ctx->client_name, CONFIG_CLIENT_CAN01, strlen(CONFIG_CLIENT_CAN01)) == 0)
    {
        Resolver_init(Demo01Plugin_initializer, &ctx->vdstate, device_wallet);
#ifdef TINY_EMBEDDED
        ctx->vdstate.options = &VehicleDatasetDemo01_options[0];
        ctx->vdstate.dataset = malloc(sizeof(can01_vehicle_dataset_t));
        Dataset_init(&ctx->vdstate);
        CanReceiver_init(ctx->vdstate.dataset, json_mutex);
        //GpsReceiver_init(json_mutex);
        ctx->using_can = 1;
        //ctx->using_gps = 1;
#else
        CanReceiver_preInitSetup();
#endif
    }
    else if (strncmp(ctx->client_name, CONFIG_CLIENT_CANOPEN01, strlen(CONFIG_CLIENT_CANOPEN01)) == 0)
    {
        Resolver_init(Demo02Plugin_initializer, &ctx->vdstate, device_wallet);
#ifdef TINY_EMBEDDED
        ctx->vdstate.options = &VehicleDatasetDemo02_options[0];
        ctx->vdstate.dataset = malloc(sizeof(canopen01_vehicle_dataset_t));
        Dataset_init(&ctx->vdstate);
        CanopenReceiver_init(ctx->vdstate.dataset, ctx->json_mutex);
        //ModbusReceiver_init(ctx->vdstate.dataset, ctx->json_mutex);
        ctx->using_canopen = 1;
        //ctx->using_modbus = 1;
#else
        CanopenReceiver_preInitSetup();
#endif
    }
    else if (strncmp(ctx->client_name, CONFIG_CLIENT_OBDII, strlen(CONFIG_CLIENT_OBDII)) == 0)
    {
        ObdiiReceiver_init("can0", ctx->json_mutex);
        ctx->using_obdii = 1;
    }

    *access_context = (Access_ctx_t)ctx;
    JSONInterface_init();
}

void Access_start(Access_ctx_t access_context)
{
    Access_ctx_t_ *ctx = (Access_ctx_t_*)access_context;

    if (ctx->using_modbus == 1) ModbusReceiver_start();
    if (ctx->using_can == 1) CanReceiver_start();
    if (ctx->using_gps == 1) GpsReceiver_start();
    if (ctx->using_canopen == 1) CanopenReceiver_start();
}

void Access_deinit(Access_ctx_t access_context)
{
    Access_ctx_t_ *ctx = (Access_ctx_t_*)access_context;

    if (ctx->using_canopen == 1) CanopenReceiver_deinit();
    if (ctx->using_gps == 1) GpsReceiver_end();
    if (ctx->using_can == 1) CanReceiver_deinit();
    if (ctx->using_modbus == 1) ModbusReceiver_stop();

    JSONInterface_deinit();
    if (ctx->vdstate.dataset != 0)
        Dataset_deinit(&ctx->vdstate);

    Timer_deinit();
}

void Access_get_vdstate(Access_ctx_t access_context, Dataset_state_t **vdstate)
{
    Access_ctx_t_ *ctx = (Access_ctx_t_*)access_context;
    *vdstate = &ctx->vdstate;
}
