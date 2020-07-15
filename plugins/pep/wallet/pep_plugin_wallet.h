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
 * \file pep_plugin_wallet.h
 * \brief
 * Resolver plugin for wallet functionality.
 * @Author Strahinja Golic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 15.06.2020. Initial version.
 * 15.07.2020. Renaming.
 ****************************************************************************/

#ifndef _PEP_PLUGIN_WALLET_H_
#define _PEP_PLUGIN_WALLET_H_

#include "pep_plugin.h"

/**
 * @fn void pep_plugin_wallet_initializer(resolver_plugin_t* action_set)
 * @brief resolver plugin initializer callback
 */
int pep_plugin_wallet_initializer(plugin_t* plugin, void* options);

#endif //_PEP_PLUGIN_WALLET_H_
