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
 * \file demo_pep_plugin_can01.h
 * \brief
 * pep_plugin plugin for CANOpen demo using relay board connected directly to
 * rpi3.
 *
 * @Author Djordje Golubovic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 * 19.06.2020. Refactoring
 ****************************************************************************/


#ifndef __CAN_02_PEP_PLUGIN_H__
#define __CAN_01_PEP_PLUGIN_H__

#include "pep_plugin.h"

/**
 * @fn void can_02_pep_plugin_initializer(pep_plugin_t* action_set)
 * @brief pep_plugin plugin initializer callback
 */
void can_02_pep_plugin_initializer(pep_plugin_t* action_set, void* options);

/**
 * @fn void can_02_pep_plugin_terminizer()
 * @brief pep_plugin plugin terminizer callback
 */
void can_02_pep_plugin_terminizer();

#endif
