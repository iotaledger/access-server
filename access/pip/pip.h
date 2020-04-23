/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \file pip.c
 * \brief
 * Implementation of Policy Information Point
 *
 * @Author Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 12.10.2018. Initial version.
 ****************************************************************************/

#ifndef _PIP_H_
#define _PIP_H_

#include "policystore.h"

#define VEHICLE_ID_LENGTH 			64
#define PIP_ERROR -1
#define INVALID_LOCK_STATE 			(unsigned char)0xFF
#define INVALID_TRUNK_STATE 		(unsigned char)0xFF
#define MAX_NUMBER_OF_DIGITS 		10



typedef enum pip_data_id {
	PIP_DOOR_ID = 0,
	PIP_TRUNK_ID = 1,
	PIP_TIME_ID = 2,
	PIP_VEHICLE_ID_ID = 3,
	PIP_EXECUTION_NUM_ID = 4,
	PIP_LOCATION_ID = 5
} pip_data_id_t;

/**
 * @fn		int pip_get_data(policy_t *pol, char *request, char *data, int length)
 *
 * @brief
 */
int pip_get_data(policy_t *pol, char *request, char *data, int length);

#endif /* _PIP_H_ */
