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

/****************************************************************************
 * \project IOTA Access
 * \file pip_plugin_gpio.h
 * \brief
 * pip_plugin plugin for Raspberry Pi GPIO
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
#ifndef __GPIO_PIP_PLUGIN_H__
#define __GPIO_PIP_PLUGIN_H__
#include "pip_plugin.h"
#include "gpio_interface.h"

/**
 * @fn void gpio_pip_plugin_initializer(pip_plugin_t* probe_set)
 * @brief pip_plugin plugin initializer callback
 */
void gpio_pip_plugin_initializer(pip_plugin_t* probe_set);

/**
 * @fn void gpio_pip_plugin_terminator()
 * @brief pip_plugin plugin terminator callback
 */
void gpio_pip_plugin_terminator();

#endif 