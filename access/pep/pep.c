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
 * \file pep.c
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 * 18.02.2019. Added enforce_request_action from resolver module.
 * 21.02.2020. Added obligations functionality.
 * 11.05.2020. Refactoring
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "pep.h"
#include "pdp.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PEP_POL_ID_SIZE 32
#define PEP_ACTION_LEN 15
#define PEP_IOTA_ADDR_LEN 81

#define PEP_ASCII_SPACE 32
#define PEP_ASCII_TAB 9
#define PEP_ASCII_CR 13
#define PEP_ASCII_LF 10

#define PEP_CHECK_WHITESPACE(x) ((x == PEP_ASCII_SPACE) || (x == PEP_ASCII_TAB) || (x == PEP_ASCII_CR) || (x == PEP_ASCII_LF) ? TRUE : FALSE)

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static pthread_mutex_t pep_mutex;
static wallet_ctx_t* dev_wallet = NULL;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static int normalize_request(char *request, int request_len, char **request_normalized)
{
	char temp[request_len];
	int charCnt = 0;

	//Check input parameters
	if (request == NULL || request_len == 0)
	{
		printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
		return 0;
	}

	//Normalize request object and save it to temp
	memset(temp, 0, request_len);

	for (int i = 0; i < request_len; i++)
	{
		if (!PEP_CHECK_WHITESPACE(request[i]))
		{
			temp[charCnt] = request[i];
			charCnt++;
		}
	}

	//Allocate memory for request_normalized
	if (*request_normalized)
	{
		free(*request_normalized);
		*request_normalized = NULL;
	}

	*request_normalized = malloc(charCnt * sizeof(char));
	if (*request_normalized == NULL)
	{
		return 0;
	}

	//Copy temp to request_normalized
	memcpy(*request_normalized, temp, charCnt * sizeof(char));

	return charCnt;
}

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static resolver_fn callback_resolver = NULL;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
bool PEP_init(wallet_ctx_t* wallet_ctx)
{
	//Set wallet
	dev_wallet = wallet_ctx;

	//Initialize mutex
	if (pthread_mutex_init(&pep_mutex, NULL) != 0)
	{
		printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
		return FALSE;
	}

	//Initialize PDP module
	if (PDP_init(dev_wallet) == FALSE)
	{
		printf("\nERROR[%s]: PDP init failed.\n", __FUNCTION__);
		return FALSE;
	}

	return TRUE;
}

bool PEP_term(void)
{
	//Clear wallet
	dev_wallet = NULL;

	//Destroy mutex
	pthread_mutex_destroy(&pep_mutex);

	//Terminate PDP module
	if (PDP_term() == FALSE)
	{
		printf("\nERROR[%s]: PDP term failed.\n", __FUNCTION__);
		return FALSE;
	}

	return TRUE;
}

bool PEP_register_callback(resolver_fn resolver)
{
	//Check input parameter
	if (resolver == NULL)
	{
		printf("\n\nERROR[%s]: Invalid input parameters.\n\n",__FUNCTION__);
		return FALSE;
	}

	//Check if it's already registered
	if (callback_resolver != NULL)
	{
		printf("\n\nERROR[%s]: Callback already registered.\n\n",__FUNCTION__);
		return FALSE;
	}

	pthread_mutex_lock(&pep_mutex);

	//Register callback
	callback_resolver = resolver;

	pthread_mutex_unlock(&pep_mutex);

	return TRUE;
}

bool PEP_unregister_callback(void)
{
	pthread_mutex_lock(&pep_mutex);

	//Unregister callback
	callback_resolver = NULL;

	pthread_mutex_unlock(&pep_mutex);

	return TRUE;
}

bool PEP_request_access(char *request)
{
	char action_value[PEP_ACTION_LEN];
	//TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
	char obligation[PDP_OBLIGATION_LEN];
	char tangle_address[PEP_IOTA_ADDR_LEN];
	char *norm_request = NULL;
	PDP_action_t action;
	PDP_decision_e ret = PDP_ERROR;

	//Check input parameter
	if (request == NULL)
	{
		printf("\n\nERROR[%s]: Invalid input parameter.\n\n",__FUNCTION__);
		return FALSE;
	}

	pthread_mutex_lock(&pep_mutex);

	memset(obligation, 0, PEP_ACTION_LEN * sizeof(char));

	action.value = action_value;
	action.wallet_address = tangle_address;
	action.device_wallet_context = dev_wallet;

	//Get normalized request
	if (normalize_request(request, strlen(request), &norm_request) == 0)
	{
		printf("\n\nERROR[%s]: Error normalizing request.\n\n",__FUNCTION__);
		pthread_mutex_unlock(&pep_mutex);
		return FALSE;
	}

	//Calculate decision and get action + obligation
	ret = PDP_calculate_decision(norm_request, obligation, &action);

	//Resolver callback
	if (callback_resolver != NULL)
	{
		callback_resolver(obligation, (void*)&action);
	}
	else
	{
		printf("\n\nERROR[%s]: Callback is not regostered.\n\n",__FUNCTION__);
		ret = PDP_ERROR;
	}

	free(norm_request);
	pthread_mutex_unlock(&pep_mutex);
	return ret == PDP_GRANT ? TRUE : FALSE;
}
