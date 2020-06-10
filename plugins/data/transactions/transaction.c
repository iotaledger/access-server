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
 * \file transaction.c
 * \brief
 * Implementation of API for transactions storage
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 10.06.2020. Initial version.
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transaction.h"
#ifdef USE_RPI
#include "rpi_trans.h"
#endif

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define TRANS_CONF_SERV_MAX_NUM 64
#define TRANS_INTERVAL_S 30
#define TRANS_TIMEOUT_S 120

/****************************************************************************
 * GLOBAL VARIBLES
 ****************************************************************************/
static TRANSACTION_serv_confirm_t service[TRANS_CONF_SERV_MAX_NUM] = {0};

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static void transaction_confirmation(uint32_t time, bool is_confirmed, pthread_t thread_id)
{
	int i;

	//Find the service
	for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++)
	{
		if (service[i].service->thread_id == thread_id)
		{
			//Found
			break;
		}
	}

	if (i == TRANS_CONF_SERV_MAX_NUM)
	{
		return;
	}

#ifdef USE_RPI
	if (!RPITRANSACTION_update_payment_status(service[i].user_id, service[i].user_id_len, service[i].action,
												service[i].action_len, is_confirmed))
	{
		printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
		return;
	}
#else
	//Add support for other platforms
#endif

	service[i].transaction_confirmed = TRUE;
}

static bool store_transaction(wallet_ctx_t* wallet_ctx, char* user_id, int user_id_len,
							char* action, int action_len, char* transaction_hash, int transaction_hash_len)
{
	int i;

	//Check input parameters
	if (user_id == NULL || action == NULL || transaction_hash == NULL)
	{
		printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
		return FALSE;
	}

#ifdef USE_RPI
	if (!RPITRANSACTION_store(user_id, user_id_len, action, action_len))
	{
		printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
		return FALSE;
	}
#else
	//Add support for other platforms
#endif

	if (wallet_check_confirmation(wallet_ctx, transaction_hash))
	{
		//Confirmed transaction
#ifdef USE_RPI
		if (!RPITRANSACTION_update_payment_status(user_id, user_id_len, action, action_len, TRUE))
		{
			printf("\nERROR[%s]: Failed to store transaction.\n", __FUNCTION__);
			return FALSE;
		}
#else
		//Add support for other platforms
#endif
	}
	else
	{
		//Not confirmed.
		
		//Clear all confirmed services
		for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++)
		{
			if (service[i].transaction_confirmed)
			{
				pthread_join(service[i].service->thread_id, NULL);
				confirmation_service_free(&service[i].service);
				service[i].service = NULL;
				service[i].action_len = 0;
				service[i].user_id_len = 0;
				service[i].transaction_confirmed = FALSE;
				free(service[i].action);
				free(service[i].user_id);
			}
		}
		
		//Start confirmation monitoring.
		for (i = 0; i < TRANS_CONF_SERV_MAX_NUM; i++)
		{
			if (service[i].service == NULL)
			{
				//First available service
				break;
			}
		}

		if (i == TRANS_CONF_SERV_MAX_NUM)
		{
			printf("\nERROR[%s]: Transaction confirmation services limit reached.\n", __FUNCTION__);
			return FALSE;
		}

		service[i].service = confirmation_service_start(wallet_ctx, transaction_hash, TRANS_INTERVAL_S,
															TRANS_TIMEOUT_S, transaction_confirmation);
		service[i].user_id = malloc(user_id_len * sizeof(char));
		service[i].user_id_len = user_id_len;
		service[i].action = malloc(action_len * sizeof(char));
		service[i].action_len = action_len;
		service[i].transaction_confirmed = FALSE;
	}

	return TRUE;
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
void TRANSACTION_init(void)
{
	PIP_register_save_tr_callback(store_transaction);
}

void TRANSACTION_term(void)
{
	PIP_unregister_save_tr_callback();
}