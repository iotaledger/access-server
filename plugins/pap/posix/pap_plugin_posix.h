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

/****************************************************************************
 * \project IOTA Access
 * \file pap_plugin_posix.h
 * \brief
 * Implementation of policy storage interface
 *
 * @Author Dejan Nedic, Strahinja Golic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 25.05.2020. Refactoring.
 * 15.07.2020. Renaming.
 ****************************************************************************/
#ifndef _PAP_PLUGIN_POSIX_H_
#define _PAP_PLUGIN_POSIX_H_

#include "pap_plugin.h"
#include "plugin.h"

int pap_plugin_posix_initializer(plugin_t *plugin, void *user_data);

#endif  //_PAP_PLUGIN_POSIX_H_
