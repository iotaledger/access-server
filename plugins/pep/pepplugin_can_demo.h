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
 * \file pepplugin_can_demo.h
 * \brief
 * PEP plugin for CAN demo. It uses relay board directly connected to rpi3 for control
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/

#ifndef _PEP_PLUGIN_CAN_DEMO_H_
#define _PEP_PLUGIN_CAN_DEMO_H_

#include "pep_plugin.h"

/**
 * @fn void pepplugincandemo_initializer(plugin_t* plugin)
 * @brief resolver plugin initializer callback
 */
int pepplugincandemo_initializer(plugin_t* plugin, void* options);

#endif
