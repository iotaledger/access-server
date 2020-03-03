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

#include "policystore.h"
#include "libauthdac.h"
#include "vehicle_dataset.h"
#include "bc_daemon.h"

#include "obdii_receiver.h"
#include "json_interface.h"
#include "config_manager.h"
#include "resolver.h"
#include "can_receiver.h"
#include "gps_receiver.h"
#include "canopen_receiver.h"
#include "user_management.h"
#include "tcp_server.h"
#include "modbus_receiver.h"

#include "psDaemon.h"

#include <time.h>
#include "globals_declarations.h"

int g_task_sleep_time;

#define Dlog_printf printf

static pthread_mutex_t lock;

static int end = 0;

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static void *AWS(void *arg);

static pthread_mutex_t *json_mutex;

static ConfigManager_config_t config;

static VehicleDataset_state_t vdstate = {0};

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

    ConfigManager_load(&config, "config.ini", argc, argv);

    g_task_sleep_time = (int)(config.thread_sleep_period * 1000);

    Resolver_set_relayboard_addr(config.relaybrd_ipaddress);
    PSDaemon_set_policy_store_address(config.policy_store_service_ip);
    PSDaemon_set_policy_store_port(config.policy_store_service_port);
    PSDaemon_set_device_id(config.device_id);

    printf("Program start\n\n");
    PolicyStore_init();

    json_mutex = JSONInterface_get_mutex();
    UserManagement_init(config.bc_hostname, config.bc_hostname_port, config.device_id);

    if (strncmp(config.client, CONFIG_CLIENT_CAN01, strlen(CONFIG_CLIENT_CAN01)) == 0)
    {
        Resolver_init_can01_remote();
        vdstate.options = &VehicleDatasetCan01_options[0];
        vdstate.dataset = (can01_vehicle_dataset_t*)malloc(sizeof(can01_vehicle_dataset_t));
        VehicleDataset_init(&vdstate);
        CanReceiver_init(config.can0_port_name, config.can1_port_name, vdstate.dataset, json_mutex);
        //GpsReceiver_init(config.gps_tty_device, json_mutex);
        using_can = 1;
        //using_gps = 1;
    }
    else if (strncmp(config.client, CONFIG_CLIENT_CANOPEN01, strlen(CONFIG_CLIENT_CANOPEN01)) == 0)
    {
        Resolver_init_canopen01();
        vdstate.options = &VehicleDatasetCanopen01_options[0];
        vdstate.dataset = (canopen01_vehicle_dataset_t*)malloc(sizeof(canopen01_vehicle_dataset_t));
        VehicleDataset_init(&vdstate);
        CanopenReceiver_init(vdstate.dataset, json_mutex, config.canopen_port_name, config.canopen_node_id);
        //ModbusReceiver_init(config.modbus_tty_device, vdstate.dataset, json_mutex);
        using_canopen = 1;
        //using_modbus = 1;
    }
    else if (strncmp(config.client, CONFIG_CLIENT_OBDII, strlen(CONFIG_CLIENT_OBDII)) == 0)
    {
        ObdiiReceiver_init("can0", json_mutex);
        using_obdii = 1;
    }

    if (vdstate.dataset != 0) {
        BlockchainDaemon_set_bc_hostname(config.bc_hostname);
        BlockchainDaemon_set_bc_hostname_port(config.bc_hostname_port);
        BlockchainDaemon_set_token_address(config.token_address);
        BlockchainDaemon_set_token_amount(config.token_amount);
        BlockchainDaemon_set_token_send_interval(config.token_send_interval);
        BlockchainDaemon_init(&vdstate);
        BlockchainDaemon_start();
    }

    JSONInterface_init(config.json_interface_ipaddr, config.json_interface_ipport, config.device_id);

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

    UserManagement_deinit();
    JSONInterface_deinit();
    if (vdstate.dataset != 0)
        VehicleDataset_deinit(&vdstate);

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


