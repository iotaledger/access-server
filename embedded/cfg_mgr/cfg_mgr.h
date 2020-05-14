/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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

#ifndef _CFG_MGR_H_
#define _CFG_MGR_H_

#include "cfg_mgr_impl.h"

int CfgMgr_init(void* in_parameter, CfgMgr_t* configuration);
int CfgMgr_get_option_string(CfgMgr_t* configuration, const char* module_name, const char* option_name, char* option_value);
int CfgMgr_get_option_int(CfgMgr_t* configuration, const char* module_name, const char* option_name, int* option_value);
int CfgMgr_get_option_float(CfgMgr_t* configuration, const char* module_name, const char* option_name, float* option_value);

#endif
