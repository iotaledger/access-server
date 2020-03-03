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
 * \project Decentralized Access Control
 * \file config_manager.h
 * \brief
 * Interface for configuration manager module.
 *
 * config.ini example:
 *  [config]
 *  relaybrd_ipaddress=192.168.3.178
 *  mainrpi_ipaddress=192.168.3.174
 *  client=can01
 *  gps_tty_device=/dev/ttyUSB0
 * end of config.ini example
 *
 * Command line parameters:
 * --relaybrd_ipaddress=<ip-address> Address of rpi zero w board, used for
 *                                   relay control.
 *
 * --mainrpi_ipaddress=<ip-address>  Address of main rpi3 board (the one on
 *                                   which this server is executed. It should
 *                                   be visible to other devices on the
 *                                   network.
 *
 * --gps_tty_device=/dev/ttyUSB0              GPS tty device Path
 *
 * --client=<can01|canopen01|obdii>           Choose client interface setup
 *
 * --symcan_ifname=<can-if-name>              CAN interface name to write to
 *
 * --can0_port_name=<can-if-name>         CAN0 interface name
 *
 * --can1_port_name=<can-if-name>         CAN1 interface name
 *
 * --token_address=<0xADDRESS>                Address which receives
 *                                            token rewards
 *
 * --token_send_interval=<int>                Token reward interval
 *
 * --token_amount=<float>                     Token reward amount
 *
 * --bc_hostname=<ip-address>                 Blockchain node address
 *
 * --bc_hostname_port=<int>                   Blockchain node port
 *
 * --policy_store_service_ip=<ip-addr>        IP address of policy store service
 *
 * --policy_store_service_port=<ip-port>      IP port for policy store service
 *
 * --json_interface_ipaddr=<ip-addr>          IP address for json interface
 *
 * --json_interface_ipport=<ip-port>          IP port for json interface
 *
 * --device_id=<device-id>                    ID of vehicle or device this instance is running on
 *
 * --canopen_port_name=<can-if-name>          CAN interface name for CANopen
 *
 * --canopen_node_id=<canopen-node-id>        CANopen node id
 *
 * --thread_sleep_period=<time-in-ms>         Period of time during which any thread sleeps/waits. Default 1ms
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.14.2019. Initial version.
 ****************************************************************************/

#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_

#define CONFIG_CLIENT_CAN01 "can01"
#define CONFIG_CLIENT_CANOPEN01 "canopen01"
#define CONFIG_CLIENT_OBDII "obdii"

typedef struct {
    char relaybrd_ipaddress[128];
    char mainrpi_ipaddress[128];
    char gps_tty_device[128];
    char client[128];
    char token_address[128];
    int token_send_interval;
    float token_amount;
    char bc_hostname[128];
    int bc_hostname_port;
    char symcan_ifname[128];
    char modbus_tty_device[128];
    char can0_port_name[8];
    char can1_port_name[8];
    char policy_store_service_ip[128];
    int policy_store_service_port;
    char json_interface_ipaddr[128];
    int json_interface_ipport;
    char device_id[67];
    char canopen_port_name[8];
    int canopen_node_id;
    float thread_sleep_period;
} ConfigManager_config_t;

int ConfigManager_load(ConfigManager_config_t* config,
                       const char* filename,
                       int argc, char** argv);

#endif
