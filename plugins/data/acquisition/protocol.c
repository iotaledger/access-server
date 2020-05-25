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
 * \file protocol.c
 * \brief
 * Implementation of data acquisition wrapper for different com. interfaces
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 18.05.2020. Initial version.
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "protocol.h"
#include "pip.h"

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static acquire_fn callback_acquire = NULL;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static bool fetch_data(char* uri, PIP_attribute_object_t* attribute_object)
{
	char temp[PROTOCOL_MAX_STR_LEN];
	char type[PROTOCOL_MAX_STR_LEN];
	char value[PROTOCOL_MAX_STR_LEN];
	char *ptr = NULL;

	//Check input parameters
	if (uri == NULL || attribute_object == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return FALSE;
	}

	//Parse uri
	if (strlen(uri) > PROTOCOL_MAX_STR_LEN)
	{
		printf("\nERROR[%s]: URI too long.\n", __FUNCTION__);
		return FALSE;
	}
	else
	{
		memcpy(temp, uri, strlen(uri));
	}

	ptr  = strtok(temp, "/");

	if (memcmp(ptr, "iota", strlen("iota")) != 0) //Only supported authority for now
	{
		printf("\nERROR[%s]: Authority not supported.\n", __FUNCTION__);
		return FALSE;
	}

	ptr  = strtok(NULL, "?");
	memcpy(type, ptr, strlen(ptr) > PROTOCOL_MAX_STR_LEN ? PROTOCOL_MAX_STR_LEN : strlen(ptr));
	ptr  = strtok(NULL, "?");
	memcpy(value, ptr, strlen(ptr) > PROTOCOL_MAX_STR_LEN ? PROTOCOL_MAX_STR_LEN : strlen(ptr));

	if (callback_acquire == NULL)
	{
		printf("\nERROR[%s]: Callback for data acquisition is not registered.\n", __FUNCTION__);
		return FALSE;
	}
	else
	{
		callback_acquire(type, value, attribute_object->type, attribute_object->value);
	}

	return TRUE;
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
bool PROTOCOL_init(void)
{
	if (PIP_register_callback(PIP_IOTA, fetch_data) != PIP_NO_ERROR)
	{
		printf("\nERROR[%s]: Register PIP callback failed.\n", __FUNCTION__);
		return FALSE;
	}

	return TRUE;
}

bool PROTOCOL_term(void)
{
	if (PIP_unregister_callback(PIP_IOTA) != PIP_NO_ERROR)
	{
		printf("\nERROR[%s]: Unregister PIP callback failed.\n");
		return FALSE;
	}

	return TRUE;
}

void PROTOCOL_register_callback(acquire_fn acquire)
{
	callback_acquire = acquire;
}

void PROTOCOL_unregister_callback()
{
	callback_acquire = NULL;
}
