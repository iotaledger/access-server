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
#include "pap.h"

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static put_fn callback_put = NULL;
static get_fn callback_get = NULL;
static has_fn callback_has = NULL;
static del_fn callback_del = NULL;

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

PAP_error_e PAP_add_policy(char *policy, int policy_size)
{
	return PAP_NO_ERROR;
}

PAP_error_e PAP_get_policy(char *policy_id, int policy_id_len, PAP_policy_t *policy)
{
	return PAP_NO_ERROR;
}

bool PAP_has_policy(char *policy_id, int policy_id_len)
{
	return FALSE;
}

PAP_error_e PAP_remove_policy(char *policy_id, int policy_id_len)
{
	return PAP_NO_ERROR;
}