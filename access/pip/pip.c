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
#define PIP_PAID 2

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static pthread_mutex_t pip_mutex;
static wallet_ctx_t* dev_wallet = NULL;

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static fetch_fn callback_fetch[PIP_MAX_AUTH_CALLBACKS] = {0};
static save_transaction_fn save_transaction = NULL;
static payment_status_fn transaction_status = NULL;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
PIP_error_e PIP_init(wallet_ctx_t* wallet_ctx)
{
	//Check input parameter
	if (wallet_ctx == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return PIP_ERROR;
	}
	else
	{
		dev_wallet = wallet_ctx;
	}

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
	dev_wallet = NULL;

	//Destroy mutex
	pthread_mutex_destroy(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_register_fetch_callback(PIP_authorities_e authority, fetch_fn fetch)
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

PIP_error_e PIP_unregister_fetch_callback(PIP_authorities_e authority)
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

PIP_error_e PIP_unregister_all_fetch_callbacks(void)
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

PIP_error_e PIP_register_save_tr_callback(save_transaction_fn save_tr)
{
	//Check input parameter
	if (save_tr == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return PIP_ERROR;
	}

	pthread_mutex_lock(&pip_mutex);

	save_transaction = save_tr;

	pthread_mutex_unlock(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_unregister_save_tr_callback(void)
{
	pthread_mutex_lock(&pip_mutex);

	save_transaction = NULL;

	pthread_mutex_unlock(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_register_payment_state_callback(payment_status_fn trans_fn)
{
	//Check input parameter
	if (trans_fn == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return PIP_ERROR;
	}

	pthread_mutex_lock(&pip_mutex);

	transaction_status = trans_fn;

	pthread_mutex_unlock(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_unregister_payment_state_callback(void)
{
	pthread_mutex_lock(&pip_mutex);

	transaction_status = NULL;

	pthread_mutex_unlock(&pip_mutex);

	return PIP_NO_ERROR;
}

PIP_error_e PIP_get_data(char* uri, PIP_attribute_object_t* attribute)
{
	char delimiter[PIP_DELIMITER_LEN] = ":";
	char temp[PIP_MAX_STR_LEN] = {0};
	char pol_id[PIP_MAX_STR_LEN] = {0};
	char type[PIP_MAX_STR_LEN] = {0};
	char value[PIP_MAX_STR_LEN] = {0};
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

	memcpy(delimiter, "/", strlen("/"));
	ptr  = strtok(NULL, delimiter);
	memcpy(pol_id, ptr, strlen(ptr));

	memcpy(delimiter, "?", strlen("?"));
	ptr  = strtok(NULL, delimiter);
	memcpy(type, ptr, strlen(ptr));

	memcpy(delimiter, "?", strlen("?"));
	ptr  = strtok(NULL, delimiter);
	memcpy(value, ptr, strlen(ptr));

	if (memcmp(type, "request.isPayed.type", strlen("request.isPayed.type")) == 0)
	{
		memcpy(attribute->type, "bool", strlen("bool"));
		if (transaction_status != NULL && transaction_status(pol_id, strlen(pol_id)) == PIP_PAID)
		{
			memcpy(attribute->value, "true", strlen("true"));
		}
		else
		{
			memcpy(attribute->value, "false", strlen("false"));
		}
	}
	else
	{
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
	}

	pthread_mutex_unlock(&pip_mutex);
	return PIP_NO_ERROR;
}

PIP_error_e PIP_store_transaction(char* policy_id, int policy_id_len,
									char* transaction_hash, int transaction_hash_len)
{
	PIP_error_e ret = PIP_ERROR;

	//Check input parameters
	if (policy_id == NULL || transaction_hash == NULL)
	{
		printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
		return ret;
	}

	pthread_mutex_lock(&pip_mutex);

	if (save_transaction)
	{
		save_transaction(dev_wallet, policy_id, policy_id_len, transaction_hash, transaction_hash_len);
		ret = PIP_NO_ERROR;
	}
	else
	{
		printf("\nERROR[%s]: Callback not registered.\n", __FUNCTION__);
	}

	pthread_mutex_unlock(&pip_mutex);

	return ret;
}
