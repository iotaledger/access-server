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
 * \project Decentralized Access Control
 * \file pdp.c
 * \brief
 * Implementation of Policy Decision Point
 *
 * @Author Dejan Nedic, Milivoje Knezevic
 *
 *
 * \notes
 *
 * \history
 * 04.09.2018. Initial version.
 * 09.11.2018 Modified to work together with PIP module
 ****************************************************************************/

#include <strings.h>
#include "pdp.h"
#include <time.h>
#include "zlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "parson.h"
#include "json_parser.h"
#include "Dlog.h"
#include "pip.h"

#define DATA_VAL_SIZE 131
#define DATA_TYPE_SIZE 21

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int resolve_attribute(policy_t *pol, int atribute_position);

char data_value[DATA_VAL_SIZE];
char data_type[DATA_TYPE_SIZE];

operation_t get_operation_new(const char *operation, int size)
{
	operation_t ret = UNDEFINED;

	if (size == 2)
	{
		if(memcmp("or", operation, size) == 0)
			ret = OR;
		else if(memcmp("eq", operation, size) == 0)
			ret = EQ;
		else if (memcmp("lt", operation, size) == 0)
			ret = LT;
		else if (memcmp("gt", operation, size) == 0)
			ret = GT;
		else if (memcmp("if", operation, size) == 0)
			ret = IF;
	}
	else if (size == 3)
	{
		if(memcmp("and", operation, size) == 0)
			ret = AND;
		else if(memcmp("not", operation, size) == 0)
			ret = NOT;
		else if(memcmp("leq", operation, size) == 0)
			ret = LEQ;
		else if(memcmp("geq", operation, size) == 0)
			ret = GEQ;
	}

	return ret;
}

int and(policy_t *pol, int attribute_list)
{
	int decision = TRUE;

	int i = 0;
	int res;
	int attribute_count = 0;
	int attribute = -1;

	attribute_count = get_array_size(attribute_list);

	for (i = 0; i < attribute_count; i++)
	{
		attribute = get_attribute_from_array(attribute_list, i);

		res = resolve_attribute(pol, attribute_list + attribute);

		if(res != ERROR)
		{
			decision = decision && res;
		}
		else
		{
			decision = FALSE;
			break;
		}
	}

	return decision;
}

int or(policy_t *pol, int attribute_list)
{
	int decision = FALSE;

	int i = 0;
	int res;
	int attribute_count = 0;
	int attribute = -1;

	attribute_count = get_array_size(attribute_list);

	for (i = 0; i < attribute_count; i++)
	{
		attribute = get_attribute_from_array(attribute_list, i);

		res = resolve_attribute(pol, attribute_list + attribute);

		if(res != ERROR)
		{
			decision = decision || res;
		}
		else
		{
			decision = FALSE;
			break;
		}
	}

	return decision;
}

int eq(policy_t *pol, int attribute_list)
{
	int ret = FALSE;
//	Dlog_printf("\neq");

	int attr1 = -1;
	int attr2 = -1;

	attr1 = get_attribute_from_array(attribute_list, 0);
	attr2 = get_attribute_from_array(attribute_list, 1);

	int type1 = json_get_value(pol->policy_c, attribute_list + attr1, "type");
	int type2 = json_get_value(pol->policy_c, attribute_list + attr2, "type");
	int value1 = json_get_value(pol->policy_c, attribute_list + attr1, "value");
	int value2 = json_get_value(pol->policy_c, attribute_list + attr2, "value");

	char *url_type = pol->policy_c + get_start_of_token(type2);
	char *url_value = pol->policy_c + get_start_of_token(value2);

	int url_size = get_size_of_token(value2);
	int url_sizet = get_size_of_token(type2);


	int data_length = 0;

	data_value[DATA_VAL_SIZE - 1] = '\0';
	data_length = pip_get_data(pol, url_value, data_value, url_size);

	if(data_length == -1)
	{
		memcpy(data_value, url_value, get_size_of_token(value2) );
		data_value[get_size_of_token(value2)] = '\0';
	}

	int type_length = 0;

	data_type[DATA_TYPE_SIZE - 1] = '\0';
	type_length = pip_get_data(pol, url_type, data_type, url_sizet);

	if(type_length == -1)
	{
		memcpy(data_type, url_type, get_size_of_token(type2));
		data_type[get_size_of_token(type2)] = '\0';
	}

	//*******************************************************************
	//PIP FOR SUBJECT REQUEST (this should be modified after PIP module is changed so it can return subject value and type
	if(data_length == -2)
	{
		memcpy(data_value, pol->policy_c + get_start_of_token(value1), get_size_of_token(value1) );
		data_value[get_size_of_token(value1)] = '\0';
	}

	if(type_length == -2)
	{
		memcpy(data_type, pol->policy_c + get_start_of_token(type1),get_size_of_token(type1));
		data_type[get_size_of_token(type1)] = '\0';
	}
	//*********************************************************************

	int size_of_type1 = get_size_of_token(type1);
	int size_of_type2 = strlen(data_type);
	int size_of_value1 = get_size_of_token(value1);
	int size_of_value2 = strlen(data_value);


	if(((size_of_type1 == size_of_type2) && (strncasecmp(pol->policy_c + get_start_of_token(type1), data_type, size_of_type1) == 0 )) &&
			((size_of_value1 == size_of_value2) && (strncasecmp(pol->policy_c + get_start_of_token(value1), data_value, size_of_value1) == 0 )))
	{
		ret = TRUE;
	}
	else
	{
		Dlog_printf("\n FALSE \n");
	}

	return ret;
}

int leq(policy_t *pol, int attribute_list)
{
	int ret = FALSE;
//	Dlog_printf("\nleq");

	int attr1 = -1;
	int attr2 = -1;

	attr1 = get_attribute_from_array(attribute_list, 0);
	attr2 = get_attribute_from_array(attribute_list, 1);

	int type1 = json_get_value(pol->policy_c, attribute_list + attr1, "type");
	int type2 = json_get_value(pol->policy_c, attribute_list + attr2, "type");
	int value1 = json_get_value(pol->policy_c, attribute_list + attr1, "value");
	int value2 = json_get_value(pol->policy_c, attribute_list + attr2, "value");

	char *url_type = pol->policy_c + get_start_of_token(type2);
	char *url_value = pol->policy_c + get_start_of_token(value2);

	int url_size = get_size_of_token(value2);
	int url_sizet = get_size_of_token(type2);


	int data_length = 0;

	data_value[DATA_VAL_SIZE - 1] = '\0';
	data_length = pip_get_data(pol, url_value, data_value, url_size);



	if(data_length == -1)
	{
		memcpy(data_value, url_value, get_size_of_token(value2) );
		data_value[get_size_of_token(value2)] = '\0';
	}

	int type_length = 0;


	data_type[DATA_TYPE_SIZE - 1] = '\0';
	type_length = pip_get_data(pol, url_type,data_type,url_sizet);


	if(type_length == -1)
	{
		memcpy(data_type,url_type,get_size_of_token(type2));
		data_type[get_size_of_token(type2)] = '\0';
	}

	//*******************************************************************
	//PIP FOR SUBJECT REQUEST (this should be modified after PIP module is changed so it can return subject value and type
	if(data_length == -2)
	{
		memcpy(data_value, pol->policy_c + get_start_of_token(value1), get_size_of_token(value1) );
		data_value[get_size_of_token(value1)] = '\0';
	}

	if(type_length == -2)
	{
		memcpy(data_type, pol->policy_c + get_start_of_token(type1),get_size_of_token(type1));
		data_type[get_size_of_token(type1)] = '\0';
	}
	//*********************************************************************

	int size_of_type1 = get_size_of_token(type1);
	int size_of_type2 = strlen(data_type);
	int size_of_value1 = get_size_of_token(value1);
	int size_of_value2 = strlen(data_value);

//	Dlog_printf("\nLEQ ");
	if(((size_of_type1 == size_of_type2) && (strncasecmp(pol->policy_c + get_start_of_token(type1), data_type, size_of_type1) == 0 )))
	{
		if(size_of_value1 < size_of_value2)
		{
			ret = TRUE;
		}else if((size_of_value1 == size_of_value2) && strncasecmp(pol->policy_c + get_start_of_token(value1), data_value, size_of_value1) <= 0 )
		{
			ret = TRUE;
		}
		else
		{

		}
	}
	else
	{

	}


	return ret;
}

int lt(policy_t *pol, int attribute_list)
{
	int ret = FALSE;
//	Dlog_printf("\nleq");

	int attr1 = -1;
	int attr2 = -1;

	attr1 = get_attribute_from_array(attribute_list, 0);
	attr2 = get_attribute_from_array(attribute_list, 1);

	int type1 = json_get_value(pol->policy_c, attribute_list + attr1, "type");
	int type2 = json_get_value(pol->policy_c, attribute_list + attr2, "type");
	int value1 = json_get_value(pol->policy_c, attribute_list + attr1, "value");
	int value2 = json_get_value(pol->policy_c, attribute_list + attr2, "value");

	char *url_type = pol->policy_c + get_start_of_token(type2);
	char *url_value = pol->policy_c + get_start_of_token(value2);

	int url_size = get_size_of_token(value2);
	int url_sizet = get_size_of_token(type2);


	int data_length = 0;

	data_value[DATA_VAL_SIZE - 1] = '\0';
	data_length = pip_get_data(pol, url_value, data_value, url_size);



	if(data_length == -1)
	{
		memcpy(data_value, url_value, get_size_of_token(value2) );
		data_value[get_size_of_token(value2)] = '\0';
	}

	int type_length = 0;


	data_type[DATA_TYPE_SIZE - 1] = '\0';
	type_length = pip_get_data(pol, url_type,data_type,url_sizet);


	if(type_length == -1)
	{
		memcpy(data_type,url_type,get_size_of_token(type2));
		data_type[get_size_of_token(type2)] = '\0';
	}

	//*******************************************************************
	//PIP FOR SUBJECT REQUEST (this should be modified after PIP module is changed so it can return subject value and type
	if(data_length == -2)
	{
		memcpy(data_value, pol->policy_c + get_start_of_token(value1), get_size_of_token(value1) );
		data_value[get_size_of_token(value1)] = '\0';
	}

	if(type_length == -2)
	{
		memcpy(data_type, pol->policy_c + get_start_of_token(type1),get_size_of_token(type1));
		data_type[get_size_of_token(type1)] = '\0';
	}
	//*********************************************************************

	int size_of_type1 = get_size_of_token(type1);
	int size_of_type2 = strlen(data_type);
	int size_of_value1 = get_size_of_token(value1);
	int size_of_value2 = strlen(data_value);

//	Dlog_printf("\nLEQ ");
	if(((size_of_type1 == size_of_type2) && (strncasecmp(pol->policy_c + get_start_of_token(type1), data_type, size_of_type1) == 0 )))
	{
		if(size_of_value1 < size_of_value2)
		{
			ret = TRUE;
		}
		else if((size_of_value1 == size_of_value2) && strncasecmp(pol->policy_c + get_start_of_token(value1), data_value, size_of_value1) < 0 )
		{
			ret = TRUE;
		}
	}


	return ret;
}

int geq(policy_t *pol, int attribute_list)
{
	int ret = FALSE;
//	Dlog_printf("\ngeq");

	int attr1 = -1;
	int attr2 = -1;

	attr1 = get_attribute_from_array(attribute_list, 0);
	attr2 = get_attribute_from_array(attribute_list, 1);

	int type1 = json_get_value(pol->policy_c, attribute_list + attr1, "type");
	int type2 = json_get_value(pol->policy_c, attribute_list + attr2, "type");
	int value1 = json_get_value(pol->policy_c, attribute_list + attr1, "value");
	int value2 = json_get_value(pol->policy_c, attribute_list + attr2, "value");

	char *url_type = pol->policy_c + get_start_of_token(type2);
	char *url_value = pol->policy_c + get_start_of_token(value2);

	int url_size = get_size_of_token(value2);
	int url_sizet = get_size_of_token(type2);


	int data_length = 0;

	data_value[DATA_VAL_SIZE - 1] = '\0';
	data_length = pip_get_data(pol, url_value, data_value, url_size);



	if(data_length == -1)
	{
		memcpy(data_value, url_value, get_size_of_token(value2) );
		data_value[get_size_of_token(value2)] = '\0';
	}

	int type_length = 0;


	data_type[DATA_TYPE_SIZE - 1] = '\0';
	type_length = pip_get_data(pol, url_type,data_type,url_sizet);


	if(type_length == -1)
	{
		memcpy(data_type,url_type,get_size_of_token(type2));
		data_type[get_size_of_token(type2)] = '\0';
	}

	//*******************************************************************
	//PIP FOR SUBJECT REQUEST (this should be modified after PIP module is changed so it can return subject value and type
	if(data_length == -2)
	{
		memcpy(data_value, pol->policy_c + get_start_of_token(value1), get_size_of_token(value1) );
		data_value[get_size_of_token(value1)] = '\0';
	}

	if(type_length == -2)
	{
		memcpy(data_type, pol->policy_c + get_start_of_token(type1),get_size_of_token(type1));
		data_type[get_size_of_token(type1)] = '\0';
	}
	//*********************************************************************

	int size_of_type1 = get_size_of_token(type1);
	int size_of_type2 = strlen(data_type);
	int size_of_value1 = get_size_of_token(value1);
	int size_of_value2 = strlen(data_value);

//	Dlog_printf("\nGEQ ");
	if(((size_of_type1 == size_of_type2) && (strncasecmp(pol->policy_c + get_start_of_token(type1), data_type, size_of_type1) == 0 )))
	{
		if(size_of_value1 > size_of_value2)
		{
			ret = TRUE;
		}
		else if((size_of_value1 == size_of_value2) && strncasecmp(pol->policy_c + get_start_of_token(value1), data_value, size_of_value1) >= 0 )
		{
			ret = TRUE;
		}
	}

	return ret;
}

int gt(policy_t *pol, int attribute_list)
{
	int ret = FALSE;
//	Dlog_printf("\nleq");

	int attr1 = -1;
	int attr2 = -1;

	attr1 = get_attribute_from_array(attribute_list, 0);
	attr2 = get_attribute_from_array(attribute_list, 1);

	int type1 = json_get_value(pol->policy_c, attribute_list + attr1, "type");
	int type2 = json_get_value(pol->policy_c, attribute_list + attr2, "type");
	int value1 = json_get_value(pol->policy_c, attribute_list + attr1, "value");
	int value2 = json_get_value(pol->policy_c, attribute_list + attr2, "value");

	char *url_type = pol->policy_c + get_start_of_token(type2);
	char *url_value = pol->policy_c + get_start_of_token(value2);

	int url_size = get_size_of_token(value2);
	int url_sizet = get_size_of_token(type2);


	int data_length = 0;

	data_value[DATA_VAL_SIZE - 1] = '\0';
	data_length = pip_get_data(pol, url_value,data_value, url_size);



	if(data_length == -1)
	{
		memcpy(data_value, url_value, get_size_of_token(value2) );
		data_value[get_size_of_token(value2)] = '\0';
	}

	int type_length = 0;


	data_type[DATA_TYPE_SIZE - 1] = '\0';
	type_length = pip_get_data(pol, url_type,data_type,url_sizet);


	if(type_length == -1)
	{
		memcpy(data_type,url_type,get_size_of_token(type2));
		data_type[get_size_of_token(type2)] = '\0';
	}

	//*******************************************************************
	//PIP FOR SUBJECT REQUEST (this should be modified after PIP module is changed so it can return subject value and type
	if(data_length == -2)
	{
		memcpy(data_value, pol->policy_c + get_start_of_token(value1), get_size_of_token(value1) );
		data_value[get_size_of_token(value1)] = '\0';
	}

	if(type_length == -2)
	{
		memcpy(data_type, pol->policy_c + get_start_of_token(type1),get_size_of_token(type1));
		data_type[get_size_of_token(type1)] = '\0';
	}
	//*********************************************************************

	int size_of_type1 = get_size_of_token(type1);
	int size_of_type2 = strlen(data_type);
	int size_of_value1 = get_size_of_token(value1);
	int size_of_value2 = strlen(data_value);

//	Dlog_printf("\nLEQ ");
	if(((size_of_type1 == size_of_type2) && (strncasecmp(pol->policy_c + get_start_of_token(type1), data_type, size_of_type1) == 0 )))
	{
		if(size_of_value1 > size_of_value2)
		{
			ret = TRUE;
		}else if((size_of_value1 == size_of_value2) && strncasecmp(pol->policy_c + get_start_of_token(value1), data_value, size_of_value1) > 0 )
		{
			ret = TRUE;
		}
	}


	return ret;
}

int resolve_attribute(policy_t *pol, int atribute_position)
{
	int ret = -1;
	operation_t opt;

	int operation = json_get_token_index_from_pos(pol->policy_c, atribute_position, "operation");
	int operation_start = -1;
	int operation_end = -1;

	int attribute_list = -1;

	if((operation != -1) && (get_start_of_token(operation) < get_end_of_token(atribute_position)))// Check only operations within this json object
	{
		attribute_list = json_get_token_index_from_pos(pol->policy_c, atribute_position, "attribute_list");

		// Check if operation is listed before or after attribure list
		if(attribute_list >= 0 && operation > attribute_list)
		{
			// If attribute list is listed first, get operation given after att. list

			int number_of_tokens = get_token_num();
			int tok_cnt = attribute_list;

			while((get_end_of_token(attribute_list) > get_start_of_token(operation)) && (tok_cnt <= number_of_tokens) && (tok_cnt >= 0))
			{
				operation = json_get_token_index_from_pos(pol->policy_c, tok_cnt, "operation");
				tok_cnt = operation;
			}
		}

		operation_start = get_start_of_token(operation);
		operation_end = get_end_of_token(operation);

		opt = get_operation_new(pol->policy_c + operation_start, operation_end - operation_start);

		switch(opt)
		{
			case OR:{
				ret = or(pol, attribute_list);
				break;
			}
			case AND:{
				ret = and(pol, attribute_list);
				break;
			}
			case EQ:{
				ret = eq(pol, attribute_list);
				break;
			}
			case LEQ:{
				ret = leq(pol, attribute_list);
				break;
			}
			case GEQ:{
				ret = geq(pol, attribute_list);
				break;
			}
			case LT:{
				ret = lt(pol, attribute_list);
				break;
			}
			case GT:{
				ret = gt(pol, attribute_list);
				break;
			}
			default:{
				ret = FALSE;
				break;
			}
		}
	}
	else
	{
		int type = json_get_token_index_from_pos(pol->policy_c, atribute_position, "type");
		if((type != -1) && (get_start_of_token(type) < get_end_of_token(atribute_position)))// Check only type within this json object
		{
			int start = get_start_of_token(type);
			int size_of_type = get_size_of_token(type);

			if((size_of_type == strlen("boolean")) && (strncasecmp(pol->policy_c + start, "boolean", strlen("boolean")) == 0))
			{
				int value = json_get_token_index_from_pos(pol->policy_c, atribute_position, "value");
				int start_of_value = get_start_of_token(value);
				int size_of_value = get_size_of_token(value);
				if((size_of_value >= strlen("true")) && (memcmp(pol->policy_c + start_of_value, "true", strlen("true")) == 0))
				{
					ret = TRUE;
				}
				else
				{
					ret = FALSE;
				}
			}
		}
	}

	return ret;
}

int get_obligation(policy_t *pol, int obl_position, char *obligation)
{
	int ret = PDP_ERROR;
	
	int type = json_get_token_index_from_pos(pol->policy_c, obl_position, "type");
	if((type != -1) && (get_start_of_token(type) < get_end_of_token(obl_position)))// Check only type within this json object
	{
		int start = get_start_of_token(type);
		int size_of_type = get_size_of_token(type);

		if((size_of_type == strlen("obligation")) &&
		(!strncasecmp(pol->policy_c + start, "obligation", strlen("obligation"))))
		{
			int value = json_get_token_index_from_pos(pol->policy_c, obl_position, "value");
			int start_of_value = get_start_of_token(value);
			int size_of_value = get_size_of_token(value);

			if(size_of_value > OBLIGATION_LEN)
			{
				size_of_value = OBLIGATION_LEN;
			}
			
			if(value >= 0)
			{
				memcpy(obligation, pol->policy_c + start_of_value, size_of_value);
				ret = value;
			}
		}
	}
	
	return ret;
}

//TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
int resolve_obligation(policy_t *pol, int obl_position, char *obligation)
{
	int ret = PDP_ERROR;
	int operation = -1;
	int attribute_list = -1;
	int operation_start = -1;
	int operation_end = -1;
	int obl_value = -1;
	operation_t opt;
	
	if(pol == NULL || obligation == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Wrong input parameters\n\n",__FUNCTION__);
		return ret;
	}
	
	// Size of obligation buff is 15
	memset(obligation, 0, sizeof(char) * OBLIGATION_LEN);

	operation = json_get_token_index_from_pos(pol->policy_c, obl_position, "operation");
	attribute_list = json_get_token_index_from_pos(pol->policy_c, obl_position, "attribute_list");
	obl_value = json_get_token_index_from_pos(pol->policy_c, obl_position, "obligations");
	
	// In case of IF operation, multiple obligations are available
	if((attribute_list >= 0) &&
	(get_start_of_token(attribute_list) < get_end_of_token(obl_position)) &&
	(operation >= 0) &&
	(get_start_of_token(operation) < get_end_of_token(obl_position)))
	{
		// Check if operation is listed before or after attribure list
		if(operation > attribute_list)
		{
			// If attribute list is listed first, get operation given after att. list
			
			int number_of_tokens = get_token_num();
			int tok_cnt = attribute_list;
			
			while((get_end_of_token(attribute_list) > get_start_of_token(operation)) && (tok_cnt <= number_of_tokens) && (tok_cnt >= 0))
			{
				operation = json_get_token_index_from_pos(pol->policy_c, tok_cnt, "operation");
				tok_cnt = operation;
			}
		}

		operation_start = get_start_of_token(operation);
		operation_end = get_end_of_token(operation);

		opt = get_operation_new(pol->policy_c + operation_start, operation_end - operation_start);

		//TODO: For now, only IF operation is supported
		switch(opt)
		{
			case IF:
				if(!resolve_attribute(pol, attribute_list))
				{
					// Take second obligation if condition is false (else branch)
					obl_value = json_get_token_index_from_pos(pol->policy_c, obl_value + 1, "obligations");
				}

				break;

			default:
				break;
		}
	}
	
	if(obl_value >= 0)
	{
		ret = get_obligation(pol, obl_value, obligation);
	}
	
	return ret;
}

//TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
pdp_decision_t pdp_calculate_decision(policy_t *pol, char *obligation, char *action)
{
	int ret = PDP_ERROR;

	if(pol == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Unknown policy\n\n",__FUNCTION__);
		return ret;
	}
	
	if(obligation == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Invalid obligation buffer\n\n",__FUNCTION__);
		return ret;
	}

	if(action == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Invalid action buffer\n\n",__FUNCTION__);
		return ret;
	}

	json_parser_init(pol->policy_c);

	int policy_goc = json_get_token_index(pol->policy_c, "policy_goc");//json_get_value(pol->policy_c, 0, "policy_goc");
	int policy_doc = json_get_token_index(pol->policy_c, "policy_doc");//json_get_value(pol->policy_c, 0, "policy_doc");
	int policy_gobl = json_get_token_index(pol->policy_c, "obligation_grant");
	int policy_dobl = json_get_token_index(pol->policy_c, "obligation_deny");

	if(policy_goc == -1)
	{
		Dlog_printf("\nPOLICY policy_goc IS NULL\n");
	}

	if(policy_doc == -1)
	{
		Dlog_printf("\nPOLICY policy_doc IS NULL\n");
	}
	
	if(policy_gobl == -1)
	{
		Dlog_printf("\nOBLIGATION obligation_grant IS NULL\n");
	}

	if(policy_dobl == -1)
	{
		Dlog_printf("\nOBLIGATION obligation_deny IS NULL\n");
	}

	int pol_goc = resolve_attribute(pol, policy_goc);
	int pol_doc = resolve_attribute(pol, policy_doc);


	ret = pol_goc + 2 * pol_doc;  // => (0, 1, 2, 3) <=> (gap, grant, deny, conflict)
	
	if(ret == 1)
	{
		//FIXME: Should action be taken for deny case also?
		int number_of_tokens = get_token_num();
		get_action(action, pol->policy_c, number_of_tokens);
		
		if(policy_gobl >= 0)
		{
			resolve_obligation(pol, policy_gobl, obligation);
		}
	}
	else if(ret == 2)
	{
		if(policy_dobl >= 0)
		{
			resolve_obligation(pol, policy_dobl, obligation);
		}
	}

	Dlog_printf("\nPOLICY GOC RESOLVED: %d", pol_goc);
	Dlog_printf("\nPOLICY DOC RESOLVED: %d", pol_doc);
	Dlog_printf("\nPOLICY RESOLVED: %d\n", ret);

	return ret;
}

