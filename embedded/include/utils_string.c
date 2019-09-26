/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
