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
 * \project Decentralized Access Control
 * \file platform_interface.h
 * \brief
 * Interface for different HW platforms for the Resolver plugin
 *
 * @Author Strahinja Golic
 *
 * \notes
 * Should be modified in accordance with platform in use.
 *
 * \history
 * 18.05.2020. Initial version.
 ****************************************************************************/
#ifndef _PLATFORM_PLUGIN_H_
#define _PLATFORM_PLUGIN_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "vehicle_dataset.h"
#include "vehicle_datasharing_dataset.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define AUTOMOTIVE

#ifdef AUTOMOTIVE
#define PLATFORM_DATASET VehicleDataset_state_t

#define PLATFORM_DATASET_OPTIONS_1 VehicleDataset01_options
#define PLATFORM_DATASET_OPTIONS_2 VehicleDataset02_options
#define PLATFORM_DATASET_OPTIONS_3 VehicleDataset03_options
#define PLATFORM_DATASET_OPTIONS_4 VehicleDataset04_options
#define PLATFORM_DATASET_OPTIONS_5 VehicleDataset05_options
#define PLATFORM_DATASET_OPTIONS_6 VehicleDataset06_options
#define PLATFORM_DATASET_OPTIONS_7 VehicleDataset07_options
#define PLATFORM_DATASET_OPTIONS_8 VehicleDataset08_options
#define PLATFORM_DATASET_OPTIONS_9 VehicleDataset09_options
#define PLATFORM_DATASET_OPTIONS_10 VehicleDataset10_options
#define PLATFORM_DATASET_OPTIONS_11 VehicleDataset11_options
#define PLATFORM_DATASET_OPTIONS_12 VehicleDataset12_options
#define PLATFORM_DATASET_OPTIONS_13 VehicleDataset13_options
#define PLATFORM_DATASET_OPTIONS_14 VehicleDataset14_options
#define PLATFORM_DATASET_OPTIONS_15 VehicleDataset15_options
#define PLATFORM_DATASET_OPTIONS_16 VehicleDataset16_options
#endif

#endif //_PLATFORM_PLUGIN_H_