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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>

#include "storage.h"
#include "asn_auth.h"
#include "bc_daemon.h"
#include "dataset.h"

#include "obdii_receiver.h"
#include "json_interface.h"
//#include "config_manager.h"
#include "cfg_mgr.h"
#include "resolver.h"
#include "demo_resolver_01.h"
#include "demo_resolver_02.h"
#include "can_receiver.h"
#include "gps_receiver.h"
#include "canopen_receiver.h"
#include "user.h"
#include "network.h"
#include "modbus_receiver.h"

#include "psDaemon.h"

#include <time.h>
#include "globals_declarations.h"
#include "timer.h"

#define MAX_CLIENT_NAME 32
#define CONFIG_CLIENT_CAN01 "can01"
#define CONFIG_CLIENT_CANOPEN01 "canopen01"
#define CONFIG_CLIENT_OBDII "obdii"

int g_task_sleep_time;

#define Dlog_printf printf

static pthread_mutex_t lock;

static int end = 0;

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static void *AWS(void *arg);

static pthread_mutex_t *json_mutex;

static Dataset_state_t vdstate = {0};

extern void Demo01Plugin_set_relayboard_addr(const char* addr);

int main(int argc, char** argv)
{
    signal(SIGINT, signal_handler);
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
    pthread_t AWSthread;
    int using_can = 0;
    int using_gps = 0;
    int using_obdii = 0;
    int using_car_gw = 0;
    int using_canopen = 0;
    int using_modbus = 0;
    char client_name[MAX_CLIENT_NAME] = "";

    CfgMgr_init("config.ini");

    int status = CfgMgr_get_option_int("config", "thread_sleep_period", &g_task_sleep_time);
    if (status != CFG_MGR_OK) g_task_sleep_time = 1000; // 1 second

    CfgMgr_get_option_string("config", "client", client_name, MAX_CLIENT_NAME);

    PSDaemon_init();

    printf("Program start\n\n");
    PolicyStore_init();

    json_mutex = JSONInterface_get_mutex();
    UserManagement_init();

    Timer_init();

    if (strncmp(client_name, CONFIG_CLIENT_CAN01, strlen(CONFIG_CLIENT_CAN01)) == 0)
    {
#ifdef TINY_EMBEDDED
        Resolver_init(Demo01Plugin_initializer, &vdstate);
        vdstate.options = &VehicleDatasetDemo01_options[0];
        vdstate.dataset = (can01_vehicle_dataset_t*)malloc(sizeof(can01_vehicle_dataset_t));
        Dataset_init(&vdstate);
        CanReceiver_init(vdstate.dataset, json_mutex);
        //GpsReceiver_init(json_mutex);
        using_can = 1;
        //using_gps = 1;
#else
        CanReceiver_preInitSetup();
#endif
    }
    else if (strncmp(client_name, CONFIG_CLIENT_CANOPEN01, strlen(CONFIG_CLIENT_CANOPEN01)) == 0)
    {
#ifdef TINY_EMBEDDED
        Resolver_init(Demo02Plugin_initializer, &vdstate);
        vdstate.options = &VehicleDatasetDemo02_options[0];
        vdstate.dataset = (canopen01_vehicle_dataset_t*)malloc(sizeof(canopen01_vehicle_dataset_t));
        Dataset_init(&vdstate);
        CanopenReceiver_init(vdstate.dataset, json_mutex);
        //ModbusReceiver_init(vdstate.dataset, json_mutex);
        using_canopen = 1;
        //using_modbus = 1;
#else
        CanopenReceiver_preInitSetup();
#endif
    }
    else if (strncmp(client_name, CONFIG_CLIENT_OBDII, strlen(CONFIG_CLIENT_OBDII)) == 0)
    {
        ObdiiReceiver_init("can0", json_mutex);
        using_obdii = 1;
    }

    if (vdstate.dataset != 0) {
        BlockchainDaemon_init(&vdstate);
        BlockchainDaemon_start();
    }

    JSONInterface_init();

    if (using_modbus == 1) ModbusReceiver_start();

    if (using_can == 1) CanReceiver_start();

    if (using_gps == 1) GpsReceiver_start();

    if (using_canopen == 1) CanopenReceiver_start();

    /* create a second thread which executes server */
    if (TCPServer_start(9998, &vdstate) != 0)
    {
        fprintf(stderr, "Error creating TCP server thread\n");
        running = 0;
        end = 1;
    }

    /* create a third thread which executes server */
    if (running != 0 && pthread_create(&AWSthread, NULL, AWS, NULL))
    {
        fprintf(stderr, "Error creating AWS thread\n");
        running = 0;
        end = 1;
    }

    while (running == 1) usleep(g_task_sleep_time);

    end = 1;

    /* wait for the third thread to finish */
    if (pthread_join(AWSthread, NULL))
    {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }

    /* wait for the second thread to finish */
    TCPServer_stop();

    if (vdstate.dataset != 0)
        BlockchainDaemon_stop();

    if (using_canopen == 1) CanopenReceiver_deinit();
    if (using_gps == 1) GpsReceiver_end();
    if (using_can == 1) CanReceiver_deinit();
    if (using_modbus == 1) ModbusReceiver_stop();

    Timer_deinit();

    UserManagement_deinit();
    JSONInterface_deinit();
    if (vdstate.dataset != 0)
        Dataset_deinit(&vdstate);

    printf("             done...\n");

    return 0;
}

static void *AWS(void *arg)
{
    int period_counter = 0;
    while(!end)
    {
        //pthread_mutex_lock(&lock);

        if (period_counter++ == (5000000 / g_task_sleep_time) || (5000000 / g_task_sleep_time) == 0) { // 5 seconds
            period_counter = 0;
            PSDaemon_do_work();
        }

        //pthread_mutex_unlock(&lock);

        usleep(g_task_sleep_time);
    }
}


