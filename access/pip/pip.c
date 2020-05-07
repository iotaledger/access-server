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
 * \file pip.c
 * \brief
 * Implementation of Policy Information Point
 *
 * @Author Milivoje Knezevic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 12.10.2018. Initial version.
 * 09.11.2018. Updated to parse request sent from PDP module
 * 07.05.2020. Refactoring
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include "pip.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PIP_DELIMITER_LEN 2

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static pthread_mutex_t pip_mutex;

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static fetch_fn callback_fetch[PIP_MAX_AUTH_CALLBACKS] = {0};

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
PIP_error_e PIP_init(void)
{
	//Initalize mutex
	if (pthread_mutex_init(&pip_mutex, NULL) != 0)
	{
		printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
		return PIP_ERROR;
	}

	return PIP_NO_ERROR;
}

PIP_error_e PIP_term(void)
{
	//Destroy mutex
	pthread_mutex_destroy(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_register_callback(PIP_authorities_e authority, fetch_fn fetch)
{
	pthread_mutex_lock(&pip_mutex);

	//Check input parameters
	if (fetch == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	if (authority > PIP_MAX_AUTH_CALLBACKS)
	{
		printf("\nERROR[%s]: Non existing authority.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	//Register callback
	if (callback_fetch[authority] == NULL)
	{
		callback_fetch[authority] = fetch;
	}
	else
	{
		printf("\nERROR[%s]: Callback is already registered.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	pthread_mutex_unlock(&pip_mutex);
	return PIP_NO_ERROR;
}

PIP_error_e PIP_unregister_callback(PIP_authorities_e authority)
{
	pthread_mutex_lock(&pip_mutex);

	//Check input parameters
	if (authority > PIP_MAX_AUTH_CALLBACKS)
	{
		printf("\nERROR[%s]: Non existing authority.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	//Unregister callback
	callback_fetch[authority] = NULL;

	pthread_mutex_unlock(&pip_mutex);
	return PIP_NO_ERROR;
}

PIP_error_e PIP_unregister_all_callbacks(void)
{
	pthread_mutex_lock(&pip_mutex);

	for (int i = 0; i < PIP_MAX_AUTH_CALLBACKS; i++)
	{
		//Unregister callback
		callback_fetch[i] = NULL;
	}

	pthread_mutex_unlock(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_get_data(char* uri, PIP_attribute_object_t* attribute)
{
	char delimiter[PIP_DELIMITER_LEN] = ":";
	char temp[PIP_MAX_STR_LEN];
	char *ptr = NULL;
	PIP_authorities_e authority;

	pthread_mutex_lock(&pip_mutex);

	//Check input parameters
	if (uri == NULL || attribute == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	/* 
	   URI format looks like: authority:policy_id/type?value
	   By parssing URI, authority can be obtained	
	 */
	if (strlen(uri) > PIP_MAX_STR_LEN)
	{
		printf("\nERROR[%s]: URI too long.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}
	else
	{
		memcpy(temp, uri, strlen(uri));
	}

	ptr  = strtok(temp, delimiter);

	if (memcmp(ptr, "iota", strlen("iota")) == 0) //Only supported authority for now
	{
		authority = PIP_IOTA;
	}
	else
	{
		printf("\nERROR[%s]: Non existing authority.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	//Fetch attribute from plugin
	if (callback_fetch[authority] != NULL)
	{
		callback_fetch[authority](uri, attribute);
	}
	else
	{
		printf("\nERROR[%s]: Callback not registered.\n", __FUNCTION__);
		pthread_mutex_unlock(&pip_mutex);
		return PIP_ERROR;
	}

	pthread_mutex_unlock(&pip_mutex);
	return PIP_NO_ERROR;
}