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
 * \file rpi_trans.h
 * \brief
 * Implementation of API for transactions storage to RPI
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
#include "rpi_trans.h"

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
bool RPITRANSACTION_store(char* policy_id, int policy_id_len)
{
	char *line = NULL;
	int line_len = 0;
	FILE *f = NULL;

	//Check input parameters
	if (policy_id == NULL)
	{
		printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
		return FALSE;
	}

	line_len = strlen("policy_id:") + policy_id_len + strlen(",validated:0\n") + 1;

	line = malloc((line_len) * sizeof(char));

	sprintf(line, "policy_id:%s,validated:0\n", policy_id);

	//Write transaction to file
	f = fopen("../../plugins/data/transactions/platforms/r_pi/bill", "a");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalida path to file.\n", __FUNCTION__);
		return FALSE;
	}

	fwrite(line, line_len, 1, f);
	fclose(f);
	free(line);

	return TRUE;
}

bool RPITRANSACTION_update_payment_status(char* policy_id, int policy_id_len, bool is_verified)
{
	char *buff = NULL;
	char *transaction = NULL;
	char *line = NULL;
	int buff_len = 0;
	int line_len = 0;
	FILE *f = NULL;

	//Check input parameters
	if (policy_id == NULL)
	{
		printf("\nERROR[%s]: Bad input prameter.\n", __FUNCTION__);
		return FALSE;
	}

	//Read transaction from file
	f = fopen("../../plugins/data/transactions/platforms/r_pi/bill", "r");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalida path to file.\n", __FUNCTION__);
		return FALSE;
	}

	fseek(f, 0L, SEEK_END);
	buff_len = ftell(f);
	fseek(f, 0L, SEEK_SET);

	buff = malloc(buff_len * sizeof(char));
	fread(buff, buff_len, 1, f);
	fclose(f);

	//Change transaction status
	line_len = strlen("policy_id:") + policy_id_len + strlen(",validated:");
	line = malloc((line_len + 1) * sizeof(char));
	sprintf(line, "policy_id:%s,validated:", policy_id);
	transaction = strstr(buff, line);
	if (is_verified)
	{
		transaction[line_len] = '1';
	}
	else
	{
		transaction[line_len] = '0';
	}

	//Write changed buffer to file
	f = fopen("../../plugins/data/transactions/platforms/r_pi/bill", "w");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalida path to file.\n", __FUNCTION__);
		free(buff);
		free(line);
		return FALSE;
	}

	fwrite(buff, buff_len, 1, f);
	fclose(f);

	free(buff);
	free(line);
	return TRUE;
}