/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
