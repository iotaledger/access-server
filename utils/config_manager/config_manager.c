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

#include "config_manager.h"

int ConfigManager_init(void* in_parameter)
{
    return ConfigManagerImplementation_init_cb(in_parameter);
}

int ConfigManager_get_option_string(const char* module_name, const char* option_name, char* option_value, size_t option_size)
{
    return ConfigManagerImplementation_get_string_cb(module_name, option_name, option_value, option_size);
}

int ConfigManager_get_option_int(const char* module_name, const char* option_name, int* option_value)
{
    return ConfigManagerImplementation_get_int_cb(module_name, option_name, option_value);
}

int ConfigManager_get_option_float(const char* module_name, const char* option_name, float* option_value)
{
    return ConfigManagerImplementation_get_float_cb(module_name, option_name, option_value);
}
