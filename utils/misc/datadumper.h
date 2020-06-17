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
 * \file datadumper.h
 * \brief
 * Data dumper and publisher interface
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

fjson_object *datadumper_init();
void datadumper_set_address(const char *new_addr);
void datadumper_set_port(int new_port);
int datadumper_dump_if_needed(int dump_period_s);
fjson_object *datadumper_get(const char *name);
pthread_mutex_t *datadumper_get_mutex();
void datadumper_deinit();
void datadumper_add_module_init_cb(fjson_object *(*json_filler)(), fjson_object **added_node, const char *name);

#endif
