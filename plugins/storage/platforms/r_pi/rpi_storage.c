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
 * \file rpi_storage.c
 * \brief
 * Implementation of policy storage API for Raspberry Pi 
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.05.2020. Initial version.
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rpi_storage.h"
#include "utils_string.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define RPI_MAX_STR_LEN 1024
#define RPI_ACCESS_ERR -1
#define RPI_POL_ID_MAX_LEN 32

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
bool RPI_store_policy(char* policy_id, char* policy_object, int policy_object_size,
						char* signature, char* public_key, char* signature_algorithm,
						char* hash_function)
{
	char pol_path[RPI_MAX_STR_LEN] = {0};
	char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
	FILE *f = NULL;

	//Check input parameters
	if ((policy_id == NULL) || (policy_object == NULL) || (policy_object_size == 0) || (signature == NULL) ||
		(public_key == NULL) || (signature_algorithm == NULL) || (hash_function == NULL))
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return FALSE;
	}

	if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
		return FALSE;
	}

	//Write policy data to a file
	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/%s.txt", pol_id_str);
	f = fopen(pol_path, "w+");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
		return FALSE;
	}

	fwrite("policy id:", strlen("policy id:"), 1, f);
	fwrite(pol_id_str, strlen(pol_id_str), 1, f);
	fwrite("\npolicy object:", strlen("\npolicy object:"), 1, f);
	fwrite(policy_object, policy_object_size, 1, f);
	fwrite("\npolicy id signature:", strlen("\npolicy id signature:"), 1, f);
	fwrite(signature, strlen(signature), 1, f);
	fwrite("\npolicy id signature public key:", strlen("\npolicy id signature public key:"), 1, f);
	fwrite(public_key, strlen(public_key), 1, f);
	fwrite("\npolicy id signature sign. algorithm:", strlen("\npolicy id signature sign. algorithm:"), 1, f);
	fwrite(signature_algorithm, strlen(signature_algorithm), 1, f);
	fwrite("\nhash function:", strlen("\nhash function:"), 1, f);
	fwrite(hash_function, strlen(hash_function), 1, f);

	fclose(f);

	//Store policy ID in stored policies file
	memset(pol_path, 0, RPI_MAX_STR_LEN * sizeof(char));
	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/stored_policies.txt");

	f = fopen(pol_path, "a");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
		return FALSE;
	}

	fwrite(pol_id_str, strlen(pol_id_str), 1, f);
	fwrite("|", strlen("|"), 1, f);

	fclose(f);

	return TRUE;
}

bool RPI_acquire_policy(char* policy_id, char* policy_object, int *policy_object_size,
						char* signature, char* public_key, char* signature_algorithm,
						char* hash_function)
{
	char pol_path[RPI_MAX_STR_LEN] = {0};
	char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
	char *buffer;
	char *substr;
	int buff_len;
	int substr_len;
	FILE *f;

	//Check input parameters
	if ((policy_id == NULL) || (policy_object == NULL) || (policy_object_size == NULL) || (signature == NULL) ||
		(public_key == NULL) || (signature_algorithm == NULL) || (hash_function == NULL))
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return FALSE;
	}

	if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
		return FALSE;
	}

	//Open file
	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/%s.txt", pol_id_str);
	f = fopen(pol_path, "r");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
		return FALSE;
	}

	//Get file size
	fseek(f, 0L, SEEK_END);
	buff_len = ftell(f);
	fseek(f, 0L, SEEK_SET);

	//Write policy to a buffer
	buffer = malloc(buff_len * sizeof(char));

	fread(buffer, buff_len * sizeof(char), 1, f);
	fclose(f);

	//Set the arguments
	substr = strstr(buffer, "policy object:");
	substr_len = strstr(buffer, "\npolicy id signature:") - (substr + strlen("policy object:"));
	memcpy(policy_object, substr + strlen("policy object:"), substr_len);

	*policy_object_size = substr_len;

	substr = strstr(buffer, "policy id signature:");
	substr_len = strstr(buffer, "\npolicy id signature public key:") - (substr + strlen("policy id signature:"));
	memcpy(signature, substr + strlen("policy id signature:"), substr_len);

	substr = strstr(buffer, "policy id signature public key:");
	substr_len = strstr(buffer, "\npolicy id signature sign. algorithm:") - (substr + strlen("policy id signature public key:"));
	memcpy(public_key, substr + strlen("policy id signature public key:"), substr_len);

	substr = strstr(buffer, "policy id signature sign. algorithm:");
	substr_len = strstr(buffer, "\nhash function:") - (substr + strlen("policy id signature sign. algorithm:"));
	memcpy(signature_algorithm, substr + strlen("policy id signature sign. algorithm:"), substr_len);

	substr = strstr(buffer, "hash function:");
	substr_len = &buffer[buff_len] - (substr + strlen("hash function:"));
	memcpy(hash_function, substr + strlen("hash function:"), substr_len);

	free(buffer);

	return TRUE;
}

bool RPI_check_if_stored_policy(char* policy_id)
{
	char pol_path[RPI_MAX_STR_LEN] = {0};
	char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};

	//Check input parameters
	if (policy_id == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return FALSE;
	}

	if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
		return FALSE;
	}

	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/%s.txt", pol_id_str);

	//Check file existance
	if(access(pol_path, F_OK) != RPI_ACCESS_ERR)
	{
		//File exists
		return TRUE;
	}
	else
	{
		//File doesn't exist
		return FALSE;
	}
}

bool RPI_flush_policy(char* policy_id)
{
	char pol_path[RPI_MAX_STR_LEN] = {0};
	char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
	char *stored_pol_buff = NULL;
	char *stored_pol_buff_new = NULL;
	int stored_pol_buff_len = 0;
	FILE *f;

	//Check input parameters
	if (policy_id == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return FALSE;
	}

	if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
		return FALSE;
	}

	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/%s.txt", pol_id_str);

	if (remove(pol_path) == 0)
	{
		//Remove policy ID from stored policies file
		memset(pol_path, 0, RPI_MAX_STR_LEN * sizeof(char));
		sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/stored_policies.txt");
		f = fopen(pol_path, "r");
		if (f == NULL)
		{
			printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
			return FALSE;
		}

		fseek(f, 0L, SEEK_END);
		stored_pol_buff_len = ftell(f);
		fseek(f, 0L, SEEK_SET);

		stored_pol_buff = malloc(stored_pol_buff_len * sizeof(char));
		stored_pol_buff_new = malloc((stored_pol_buff_len - (strlen(pol_id_str) + 1)) * sizeof(char)); //Add +1 for "|"

		fread(stored_pol_buff_len, stored_pol_buff_len, 1, f);

		fclose(f);

		memcpy(stored_pol_buff_new, stored_pol_buff, (strstr(stored_pol_buff, pol_id_str) - stored_pol_buff));
		memcpy(&stored_pol_buff_new[strstr(stored_pol_buff, pol_id_str) - stored_pol_buff],
				(strstr(stored_pol_buff, pol_id_str) + (strlen(pol_id_str) + 1)), //Add +1 for "|"
				(&stored_pol_buff[stored_pol_buff_len] - ((strstr(stored_pol_buff, pol_id_str) + (strlen(pol_id_str) + 1)))));

		f = fopen(pol_path, "w");
		if (f == NULL)
		{
			printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
			return FALSE;
		}

		fwrite(stored_pol_buff_new, stored_pol_buff_len - (strlen(pol_id_str) + 1), 1, f); //Add +1 for "|"

		fclose(f);

		free(stored_pol_buff);
		free(stored_pol_buff_new);

		//Success
		return TRUE;
	}
	else
	{
		//Fail
		return FALSE;
	}
}

int RPI_get_pol_obj_len(char* policy_id)
{
	char pol_path[RPI_MAX_STR_LEN] = {0};
	char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
	char *buffer;
	int buff_len;
	int ret = 0;
	FILE *f;

	//Check input parameters
	if (policy_id == NULL)
	{
		printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
		return ret;
	}

	if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS)
	{
		printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
		return ret;
	}

	//Open file
	sprintf(pol_path, "../plugins/storage/platforms/r_pi/policies/%s.txt", pol_id_str);
	f = fopen(pol_path, "r");
	if (f == NULL)
	{
		printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
		return ret;
	}

	//Get file size
	fseek(f, 0L, SEEK_END);
	buff_len = ftell(f);
	fseek(f, 0L, SEEK_SET);

	//Write policy to a buffer
	buffer = malloc(buff_len * sizeof(char));

	fread(buffer, buff_len * sizeof(char), 1, f);
	fclose(f);

	ret = &buffer[buff_len] - (strstr(buffer, "policy object:") + strlen("policy object:"));

	return ret;
}

char* RPI_get_stored_pol_info_file(void)
{
	return "../plugins/storage/platforms/r_pi/policies/stored_policies.txt";
}