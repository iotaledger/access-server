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
 * \file plugin_01.h
 * \brief
 * Resolver plugin for CAN demo. There are two variants, one is using relay
 * board directly connected to rpi3, and other is using relay board
 * connected through TCP socket.
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/

#ifndef __DEMO_PLUGIN_01_H__
#define __DEMO_PLUGIN_01_H__

#include "resolver.h"

/**
 * @fn void DemoPlugin01_initializer(resolver_plugin_t* action_set)
 * @brief resolver plugin initializer callback
 */
void DemoPlugin01_initializer(resolver_plugin_t* action_set);

/**
 * @fn void DemoPlugin01_initializer_tcp(resolver_plugin_t* action_set)
 * @brief resolver plugin initializer callback
 */
void DemoPlugin01_initializer_tcp(resolver_plugin_t* action_set);

/**
 * @fn void DemoPlugin01_set_relayboard_addr(const char* addr)
 * @brief sets 	IP address for relayboard access
 * @param addr 	IP address for relayboard access
 */
void DemoPlugin01_set_relayboard_addr(const char* addr);

#endif
