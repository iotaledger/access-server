/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \project Policy Store
 * \file utils_string.h
 * \brief
 * Utilities when operating with strings
 *
 * @Author Robert Cernjanski
 *
 * \notes
 *
 * \history
 * 19.04.2019. Initial version.
 ****************************************************************************/

#include "stdlib.h"
#include "utils_string.h"


int hex_to_str(char *hex, char *str, int hex_len)
{
	if (hex == NULL)
		return UTILS_STRING_ERR_HEX;

	if (str == NULL)
		return UTILS_STRING_ERR_STR;

	unsigned char temp;

	for(int i = 0 ; i < hex_len; i++)
	{
		temp = (hex[i] >> 4) & 0x0f;

		if((temp >= 0) && (temp <= 9))
		{
			str[2 * i] = '0' + temp;
		}
		else if((temp >= 10) && (temp <= 15))
		{
			str[2 * i] = 'A' + temp - 10;
		}
		else
		{
			return UTILS_STRING_ERR;
		}

		temp = hex[i] & 0x0f;

		if(temp >= 0 && temp <= 9)
		{
			str[2 * i + 1] = '0' + temp;
		}
		else if(temp >= 10 && temp <= 15)
		{
			str[2 * i + 1] = 'A' + temp - 10;
		}
		else
		{
			return UTILS_STRING_ERR;
		}
	}
	return UTILS_STRING_SUCCESS;
}

int str_to_hex(char *str, char *hex, int str_len)
{
	if (str == NULL)
		return UTILS_STRING_ERR_STR;

	if (hex == NULL)
		return UTILS_STRING_ERR_HEX;

	if ((str_len % 2) != 0) // str_len must be even
		return UTILS_STRING_ERR_STR_LEN;

	int index = 0;
	while (index < str_len) {
		char c = str[index];
		int value = 0;

		if(c >= '0' && c <= '9')
			value = (c - '0');
		else if (c >= 'A' && c <= 'F')
			value = (10 + (c - 'A'));
		else if (c >= 'a' && c <= 'f')
			value = (10 + (c - 'a'));
		else
			return UTILS_STRING_ERR;

		hex[(index / 2)] += value << (((index + 1) % 2) * 4);
		index++;
	}

	return UTILS_STRING_SUCCESS;
}
