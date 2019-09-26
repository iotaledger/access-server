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

#ifndef _UTILS_STRING_H_
#define _UTILS_STRING_H_

#define UTILS_STRING_SUCCESS		0 	// no problem
#define UTILS_STRING_ERR			-1 	// unknown problem
#define UTILS_STRING_ERR_HEX    	1 	// problem with hex value
#define UTILS_STRING_ERR_STR		2 	// problem with str value
#define UTILS_STRING_ERR_STR_LEN 	4 	// problem with str length

/**
 * @fn 		int hex_to_str(char *hex, char *str, int hex_len)
 *
 * @brief 	Function that converts hex value to human readable string
 *
 * @param 	hex		hexadecimal value
 * @param 	str		output string not termnated with '\0'
 * 					(length must be minimum hex_len * 2)
 * @param 	hex_len	length of hexadecimal value
 *
 * @return	0 if there is no problem
 * 			1 if hex pointer is invalid
 * 			2 if str pointer is invalid
 * 			-1 if there is unknown problem
 */
int hex_to_str(char *hex, char *str, int hex_len);

/**
 * @fn 		int str_to_hex(char *str, char *hex, int str_len)
 *
 * @brief 	Function that converts human readable string value to hexadecimal
 *
 * @param 	str		intput string
 * @param 	hex		output hexadecimal value not termnated with '\0'
 * 					(length must be minimum str_len / 2)
 * @param 	str_len	length of string
 *
 * @return	0 if there is no problem
 * 			1 if hex pointer is invalid
 * 			2 if str pointer is invalid
 * 			3 if string length is not even number
 * 			-1 if there is unknown problem
 */
int str_to_hex(char *str, char *hex, int str_len);


#endif // #ifndef _UTILS_STRING_H_
