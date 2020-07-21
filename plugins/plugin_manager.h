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

#ifndef _PLUGIN_MANAGER_H_
#define _PLUGIN_MANAGER_H_

#include "plugin.h"

typedef struct {
  plugin_t* plugins;
  size_t plugins_num;
  size_t plugins_max_num;
} plugin_manager_t;

int plugin_manager_init(plugin_manager_t*, size_t plugins_max_num);
int plugin_manager_register(plugin_manager_t*, plugin_t*);
int plugin_manager_get(plugin_manager_t*, size_t idx, plugin_t**);

#endif //_PLUGIN_MANAGER_H_
