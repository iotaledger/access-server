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
 * \file bc_daemon.h
 * \brief
 * Blockchain daemon header file
 *
 * @Author Robert Cernjanski
 *
 * \notes
 *
 * \history
 * 06.05.2019. Initial version.
 ****************************************************************************/

#ifndef _BC_DAEMON_H_
#define _BC_DAEMON_H_

#include "vehicle_dataset.h"

void BlockchainDaemon_init(VehicleDataset_state_t *dataset);
void BlockchainDaemon_start();
void BlockchainDaemon_stop();

void BlockchainDaemon_set_token_address(const char* new_token_address);
void BlockchainDaemon_set_token_send_interval(int send_interval_us);
void BlockchainDaemon_set_token_amount(float new_amount);
void BlockchainDaemon_set_bc_hostname(const char* new_bc_hostname);
void BlockchainDaemon_set_bc_hostname_port(int port);

#endif // _BC_MODULE_H_
