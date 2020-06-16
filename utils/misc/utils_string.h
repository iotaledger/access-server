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
