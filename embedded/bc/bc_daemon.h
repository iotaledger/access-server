/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
