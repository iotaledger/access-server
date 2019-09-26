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
 * \file json_interface.h
 * \brief
 * JSON dumper interface for can_receiver and gps_receiver
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#ifndef _JSON_INTERFACE_H_
#define _JSON_INTERFACE_H_

#include <pthread.h>
#include "libfastjson/json.h"

fjson_object* JSONInterface_init(const char* ipaddress, int port, const char* device_id);
void JSONInterface_set_address(const char* new_addr);
void JSONInterface_set_port(int new_port);
int JSONInterface_dump_if_needed(int dump_period_s, int dump_to_ipfs);
fjson_object* JSONInterface_get(const char* name);
pthread_mutex_t* JSONInterface_get_mutex();
void JSONInterface_deinit();
void JSONInterface_add_module_init_cb(fjson_object* (*json_filler)(), fjson_object** added_node, const char* name);

#endif
