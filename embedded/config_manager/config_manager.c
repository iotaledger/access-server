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

/****************************************************************************
 *  -----------------------------------------------------
 *  https://xain.io/
 *  -----------------------------------------------------
 *
 * \project Decentralized Access Control
 * \file config_manager.h
 * \brief
 * Implementation of configuration manager module.
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.24.2019. Initial version.
 ****************************************************************************/
#include "config_manager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ketopt.h"
#include "ini.h"


static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    ConfigManager_config_t* pconfig = (ConfigManager_config_t*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("config", "relaybrd_ipaddress")) {
        strncpy(pconfig->relaybrd_ipaddress, value, sizeof(pconfig->relaybrd_ipaddress));
    } else if (MATCH("config", "mainrpi_ipaddress")) {
        strncpy(pconfig->mainrpi_ipaddress, value, sizeof(pconfig->mainrpi_ipaddress));
    } else if (MATCH("config", "gps_tty_device")) {
        strncpy(pconfig->gps_tty_device, value, sizeof(pconfig->gps_tty_device));
    } else if (MATCH("config", "token_address")) {
        strncpy(pconfig->token_address, value, sizeof(pconfig->token_address));
    } else if (MATCH("config", "token_send_interval")) {
        pconfig->token_send_interval = atoi(value);
    } else if (MATCH("config", "token_amount")) {
        pconfig->token_amount = atof(value);
    } else if (MATCH("config", "bc_hostname")) {
        strncpy(pconfig->bc_hostname, value, sizeof(pconfig->bc_hostname));
    } else if (MATCH("config", "bc_hostname_port")) {
        pconfig->bc_hostname_port = atoi(value);
    } else if (MATCH("config", "client")) {
        strncpy(pconfig->client, value, sizeof(pconfig->client));
    } else if (MATCH("config", "symcan_ifname")) {
        strncpy(pconfig->symcan_ifname, value, sizeof(pconfig->symcan_ifname));
    } else if (MATCH("config", "modbus_tty_device")) {
        strncpy(pconfig->modbus_tty_device, value, sizeof(pconfig->modbus_tty_device));
    } else if (MATCH("config", "can0_port_name")) {
        strncpy(pconfig->can0_port_name, value, sizeof(pconfig->can0_port_name));
    } else if (MATCH("config", "can1_port_name")) {
        strncpy(pconfig->can1_port_name, value, sizeof(pconfig->can1_port_name));
    } else if (MATCH("config", "policy_store_service_ip")) {
        strncpy(pconfig->policy_store_service_ip, value, sizeof(pconfig->policy_store_service_ip));
    } else if (MATCH("config", "policy_store_service_port")) {
        pconfig->policy_store_service_port = atoi(value);
    } else if (MATCH("config", "json_interface_ipaddr")) {
        strncpy(pconfig->json_interface_ipaddr, value, sizeof(pconfig->json_interface_ipaddr));
    } else if (MATCH("config", "json_interface_ipport")) {
        pconfig->json_interface_ipport = atoi(value);
    } else if (MATCH("config", "device_id")) {
        strncpy(pconfig->device_id, value, sizeof(pconfig->device_id));
    } else if (MATCH("config", "canopen_port_name")) {
        strncpy(pconfig->canopen_port_name, value, sizeof(pconfig->canopen_port_name));
    } else if (MATCH("config", "canopen_node_id")) {
        pconfig->canopen_node_id = atoi(value);
    } else if (MATCH("config", "thread_sleep_period")) {
        pconfig->thread_sleep_period = atof(value);
    } else {
        return 0;
    }

    return 1;
}

static ko_longopt_t longopts[] = {
    { "relaybrd_ipaddress", ko_required_argument, 301 },
    { "mainrpi_ipaddress", ko_required_argument, 302 },
    { "client", ko_required_argument, 304 },
    { "modbus_tty_device", ko_required_argument, 305 },
    { "symcan_ifname", ko_required_argument, 306 },
    { "gps_tty_device", ko_required_argument, 307 },
    { "token_address", ko_required_argument, 308 },
    { "token_send_interval", ko_required_argument, 309 },
    { "token_amount", ko_required_argument, 310 },
    { "bc_hostname", ko_required_argument, 311 },
    { "bc_hostname_port", ko_required_argument, 312 },
    { "canopen_port_name", ko_required_argument, 313 },
    { "canopen_node_id", ko_required_argument, 314 },
    { "can0_port_name", ko_required_argument, 315 },
    { "can1_port_name", ko_required_argument, 316 },
    { "policy_store_service_ip", ko_required_argument, 317 },
    { "json_interface_ipaddr", ko_required_argument, 318 },
    { "json_interface_ipport", ko_required_argument, 319 },
    { "device_id", ko_required_argument, 320 },
    { "thread_sleep_period", ko_required_argument, 321 },
    { "policy_store_service_port", ko_required_argument, 322 },

    { NULL, 0, 0 }
};

int ConfigManager_load(ConfigManager_config_t* config,
                       const char* filename,
                       int argc, char** argv)
{
    // Print command line options help always
    printf("Usage:\n\n--relaybrd_ipaddress=<ip-address>\n"
           "\t\tAddress of rpi zero w board, used for relay control.\n\n"
           "--mainrpi_ipaddress=<ip-address>\n"
           "\t\tAddress of main rpi3 board (the one on which this server\n"
           "\t\tis executed. It should be visible to other devices on the\n"
           "\t\tnetwork.\n\n"
           "--gps_tty_device=/dev/ttyUSB0\n"
           "\t\tGPS tty device Path\n\n"
           "--client=<can01|canopen01|obdii>\n"
           "\t\tChoose client interface setup\n\n"
           "--modbus_tty_device=/dev/ttyUSB1\n"
           "\t\tmodbus tty device path\n\n"
           "--symcan_ifname=<can-if-name>\n"
           "\t\tCAN interface name to write to\n\n"
           "--can0_port_name=<can-if-name>\n"
           "\t\tCAN0 interface name\n\n"
           "--can1_port_name=<can-if-name>\n"
           "\t\tCAN1 interface name\n\n"
           "--token_address=<0xADDRESS>\n"
           "\t\tAddress which receives token rewards\n\n"
           "--token_send_interval=<int>\n"
           "\t\tToken reward interval\n\n"
           "--token_amount=<float>\n"
           "\t\tToken reward amount\n\n"
           "--bc_hostname=<ip-address>\n"
           "\t\tBlockchain node address\n\n"
           "--bc_hostname_port=<int>\n"
           "\t\tBlockchain node port\n\n"
           "--policy_store_service_ip=<ip-port>\n"
           "\t\tIP address of policy store service\n\n"
           "--json_interface_ipaddr=<ip-addr>\n"
           "\t\tIP address for json interface\n\n"
           "--json_interface_ipport=<ip-port>\n"
           "\t\tIP port for json interface\n\n"
           "--device_id=<device-id>\n"
           "\t\tID of vehicle or device this instance is running on\n\n"
           "--canopen_port_name=<can-if-name>\n"
           "\t\tCAN interface name for CANopen\n\n"
           "--canopen_node_id=<canopen-node-id>\n"
           "\t\tCANopen node id\n\n"
           "--thread_sleep_period=<time-in-ms>\n"
           "\t\tPeriod of time during which any thread sleeps/waits. Default 1ms.\n\n");


    // Initial safe options, if there is no .ini file or command line
    // arguments.
    strncpy(config->relaybrd_ipaddress, "127.0.0.1", sizeof(config->relaybrd_ipaddress));
    strncpy(config->mainrpi_ipaddress, "127.0.0.1", sizeof(config->mainrpi_ipaddress));
    strncpy(config->gps_tty_device, "/dev/ttyUSB0", sizeof(config->gps_tty_device));
    strncpy(config->modbus_tty_device, "/dev/ttyUSB1", sizeof(config->modbus_tty_device));
    strncpy(config->token_address, "0x9A61Ad437E90022946A72abBa55fea3FE7a42dd", 128);
    config->token_send_interval = 10;
    config->token_amount = 0.001;
    strncpy(config->bc_hostname, "http://34.77.82.182", 128);
    config->bc_hostname_port = 8888;
    strncpy(config->client, CONFIG_CLIENT_CAN01, sizeof(config->client));
    strncpy(config->client, CONFIG_CLIENT_CAN01, sizeof(config->client));
    strncpy(config->symcan_ifname, "vcan0", sizeof(config->symcan_ifname));
    strncpy(config->can0_port_name, "can0", sizeof(config->can0_port_name));
    strncpy(config->can1_port_name, "can1", sizeof(config->can1_port_name));
    strncpy(config->policy_store_service_ip, "193.239.219.4", sizeof(config->policy_store_service_ip));
    config->policy_store_service_port = 6007;
    strncpy(config->json_interface_ipaddr, "127.0.0.1", sizeof(config->json_interface_ipaddr));
    config->json_interface_ipport = 12345;
    strncpy(config->device_id, "123", sizeof(config->device_id));
    strncpy(config->canopen_port_name, "can1", sizeof(config->canopen_port_name));
    config->canopen_node_id = 42;
    config->thread_sleep_period = 1.0;

    if (ini_parse(filename, handler, config) < 0) {
        printf("Can't load '%s'\n", filename);
    }

    ketopt_t opt = KETOPT_INIT;
    int i, c;
    while ((c = ketopt(&opt, argc, argv, 1, "", longopts)) >= 0) {
        switch (c) {
            case 301:
                strncpy(config->relaybrd_ipaddress, opt.arg, sizeof(config->relaybrd_ipaddress));
            break;
            case 302:
                strncpy(config->mainrpi_ipaddress, opt.arg, sizeof(config->mainrpi_ipaddress));
            break;
            case 304:
                strncpy(config->client, opt.arg, sizeof(config->client));
            break;
            case 305:
                strncpy(config->modbus_tty_device, opt.arg, sizeof(config->modbus_tty_device));
            break;
            case 306:
                strncpy(config->symcan_ifname, opt.arg, sizeof(config->symcan_ifname));
            break;
            case 307:
                strncpy(config->gps_tty_device, opt.arg, sizeof(config->gps_tty_device));
            break;
            case 308:
                strncpy(config->token_address, opt.arg, sizeof(config->token_address));
            break;
            case 309:
                config->token_send_interval = atoi(opt.arg);
            break;
            case 310:
                config->token_amount = atof(opt.arg);
            break;
            case 311:
                strncpy(config->bc_hostname, opt.arg, sizeof(config->bc_hostname));
            break;
            case 312:
                config->bc_hostname_port = atoi(opt.arg);
            break;
            case 313:
                strncpy(config->canopen_port_name, opt.arg, sizeof(config->canopen_port_name));
            break;
            case 314:
                config->canopen_node_id = atoi(opt.arg);
            break;
            case 315:
                strncpy(config->can0_port_name, opt.arg, sizeof(config->can0_port_name));
            break;
            case 316:
                strncpy(config->can1_port_name, opt.arg, sizeof(config->can1_port_name));
            break;
            case 317:
                strncpy(config->policy_store_service_ip, opt.arg, sizeof(config->policy_store_service_ip));
            break;
            case 322:
                config->policy_store_service_port = atoi(opt.arg);
            break;
            case 318:
                strncpy(config->json_interface_ipaddr, opt.arg, sizeof(config->json_interface_ipaddr));
            break;
            case 319:
                config->json_interface_ipport = atoi(opt.arg);
            break;
            case 320:
                strncpy(config->device_id, opt.arg, sizeof(config->device_id));
            break;
            case 321:
                config->thread_sleep_period = atof(opt.arg);
            break;
        }
    }

    return 0;
}
