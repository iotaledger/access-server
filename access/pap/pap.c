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
 * \file pap.c
 * \brief
 * Implementation of Policy Administration Point
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 28.04.2020. Initial version.
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pap.h"
#include "jsmn.h"
#include "utils_string.h"
#include "apiorig.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PAP_MAX_TOKENS 1024
#define PAP_ECDSA_PK_SIZE 32

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static put_fn callback_put = NULL;
static get_fn callback_get = NULL;
static has_fn callback_has = NULL;
static del_fn callback_del = NULL;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
void get_public_key_from_user(char *pk)
{
	//@TODO: poll user to acquire public key directly from it's creator
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
PAP_error_e PAP_register_callbacks(put_fn put, get_fn get, has_fn has, del_fn del)
{
	/*
	Plugin doesn't need to use all callbacks, so any of parrameters can be NULL.
	Therefore, we will not check input parameters at function entry.
	*/
	callback_put = put;
	callback_get = get;
	callback_has = has;
	callback_del = del;
	return PAP_NO_ERROR;
}

PAP_error_e PAP_unregister_callbacks(void)
{
	callback_put = NULL;
	callback_get = NULL;
	callback_has = NULL;
	callback_del = NULL;
	return PAP_NO_ERROR;
}

PAP_error_e PAP_add_policy(char *signed_policy, int signed_policy_size)
{
	char policy_id[PAP_POL_ID_MAX_LEN + 1] = {0};
	char public_key[PAP_ECDSA_PK_SIZE] = {0};
	char *policy = NULL;
	int tok_num = 0;
	unsigned long long policy_size = 0;
	PAP_policy_object_t policy_object;
	PAP_policy_id_signature_t policy_id_signature;
	PAP_hash_functions_e hash_fn;
	jsmn_parser parser;
	jsmntok_t tokens[PAP_MAX_TOKENS];

	//Check input parameters
	if (signed_policy == NULL || signed_policy_size == 0)
	{
		printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	policy = malloc(signed_policy_size * sizeof(char)); //Worst case scenario

	//Verify policy signature
	get_public_key_from_user(public_key);
	if (crypto_sign_open(policy, &policy_size, signed_policy, signed_policy_size, public_key) != 0)
	{
		//Signature verification failed
		printf("\nERROR[%s]: Policy signature can not be verified.\n", __FUNCTION__);
		free(policy);
		return PAP_ERROR;

	}

	//Parse policy
	jsmn_init(&parser);

	tok_num = jsmn_parse(&parser, policy, policy_size, tokens, PAP_MAX_TOKENS);
	if (tok_num <= 0)
	{
		printf("\nERROR[%s]: Parsing policy failed.\n", __FUNCTION__);
		free(policy);
		return PAP_ERROR;
	}

	for (int i = 0; i < tok_num; i++)
	{
		//Get policy_id
		if (strncmp(&policy[tokens[i].start], "policy_id", strlen("policy_id")) == 0)
		{
			if ((tokens[i + 1].end - tokens[i + 1].start) <= PAP_POL_ID_MAX_LEN * 2)
			{
				if (str_to_hex(&policy[tokens[i + 1].start], policy_id, (tokens[i + 1].end - tokens[i + 1].start)) != UTILS_STRING_SUCCESS)
				{
					printf("\nERROR[%s]: Could not convert string to hex value.\n", __FUNCTION__);
					free(policy);
					return PAP_ERROR;
				}
			}
			else
			{
				printf("\nERROR[%s]: Size of policy id does not match supported hash functions.\n", __FUNCTION__);
				free(policy);
				return PAP_ERROR;
			}
		}

		//Get policy_object
		if (strncmp(&policy[tokens[i].start], "policy_object", strlen("policy_object")) == 0)
		{
			//We will only provide address in buffer, plugin should take care of memory allocation
			policy_object.policy_object = &policy[tokens[i + 1].start];
			policy_object.policy_object_size = tokens[i + 1].end - tokens[i + 1].start;
		}

		//Get hash_fn
		if (strncmp(&policy[tokens[i].start], "hash_function", strlen("hash_function")) == 0)
		{
			if (strncmp(&policy[tokens[i + 1].start], "sha-256", strlen("sha-256")) == 0)
			{
				hash_fn = PAP_SHA_256;
			}
			else
			{
				printf("\nERROR[%s]: Hash function not supported.\n", __FUNCTION__);
				return PAP_ERROR;
			}
		}
	}

	//@TODO: Check what policy_id_signature should contain

	//Put policy in storage
	if (callback_put != NULL)
	{
		callback_put(policy_id, policy_object, policy_id_signature, hash_fn);
	}
	else
	{
		printf("\nERROR[%s]: Callback is not registered.\n", __FUNCTION__);
		free(policy);
		return PAP_ERROR;
	}

	free(policy);
	return PAP_NO_ERROR;
}

PAP_error_e PAP_get_policy(char *policy_id, int policy_id_len, PAP_policy_t *policy)
{
	char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

	//Check input parameters
	if (policy_id == NULL || policy_id_len == 0 || policy == NULL)
	{
		printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	//Get policy ID hex value
	if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert string to hex value.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	memcpy(policy->policy_ID, policy_id_hex, PAP_POL_ID_MAX_LEN + 1);

	//Get policy from storage
	if (callback_get != NULL)
	{
		callback_get(policy_id_hex, &(policy->policy_object), &(policy->policy_id_signature), &(policy->hash_function));
	}
	else
	{
		printf("\nERROR[%s]: Callback is not registered.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	return PAP_NO_ERROR;
}

bool PAP_has_policy(char *policy_id, int policy_id_len)
{
	char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

	//Check input parameters
	if (policy_id == NULL || policy_id_len == 0)
	{
		printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
		return FALSE;
	}

	//Get policy ID hex value
	if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert string to hex value.\n", __FUNCTION__);
		return FALSE;
	}

	//Check if policy is already in the storage
	if (callback_has != NULL)
	{
		return callback_has(policy_id_hex);
	}
	else
	{
		printf("\nERROR[%s]: Callback is not registered.\n", __FUNCTION__);
		return FALSE;
	}
}

PAP_error_e PAP_remove_policy(char *policy_id, int policy_id_len)
{
	char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

	//Check input parameters
	if (policy_id == NULL || policy_id_len == 0)
	{
		printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	//Get policy ID hex value
	if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert string to hex value.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	//Delete policy from storage
	if (callback_del != NULL)
	{
		callback_del(policy_id_hex);
	}
	else
	{
		printf("\nERROR[%s]: Callback is not registered.\n", __FUNCTION__);
		return PAP_ERROR;
	}

	return PAP_NO_ERROR;
}