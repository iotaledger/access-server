/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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

/***************************************************************************
 * Project:     IOTA Access
 *
 * Module:      Policy validation tool
 *
 * Info:        This module is created in order to validate or
 *              discard policy, also it has functionality to
 *              perform policy optimisation.
 *
 * File:        optimizator.c
 *
 * Designed-by: Strahinja Golic
 *
 * History:     25.03.2020. - Initial version
 *****************************************************************************/

/***************************************************************************
 * INCLUDES
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimizator.h"

#define JSMN_HEADER
#include "jsmn.h"

/***************************************************************************
 * DEFINES
****************************************************************************/
#define OPTIMIZATOR_MAX_TOKENS 256
#define OPTIMIZATOR_MAX_BUFFER 4*1024
#define OPTIMIZATOR_CHAR_PER_ITERATION 3
#define OPTIMIZATOR_REMOVAL_BRACKET_ELEMENTS 6 //2 variables + 2 bracket + 1 operator inside bracket + 1 operator outside bracket
#define OPTIMIZATIR_MAX_OPERATION_LEN 4
#define OPTIMIZATOR_DOC_GOC_INDENTATION 2
#define OPTIMIZATOR_FIRST_NON_WHITESPACE_ASCII_CHAR 33
#define OPTIMIZATOR_ASCII_SPACE 32

/***************************************************************************
 * GLOBAL VARIABLES
****************************************************************************/
static int tok_num = 0;
static Optimizator_symbol_t sub_var_symbol_cnt = 0;
static Optimizator_function_list_elem_t *doc_head = NULL;
static Optimizator_function_list_elem_t *goc_head = NULL;

/***************************************************************************
 * FUNCTION IMPLEMENTATIONS
****************************************************************************/
 /*
 *  Function: get_list_size
 *  Description: Get number of elements in the lsit
 *  Parameters: list - the list
 *  Returns: number of elements
 */
int get_list_size(Optimizator_function_list_elem_t *list)
{
	int ret = 0;
	Optimizator_function_list_elem_t *temp = list;

	while (temp)
	{
		ret++;
		temp = temp->next;
	}

	return ret;
}

 /*
 *  Function: list_cmp
 *  Description: Check if two lists are equal
 *  Parameters: list_1 - first list
 *              list_2 - second lisr
 *  Returns: TRUE if equal, FALSE if not
 */
bool list_cmp(Optimizator_function_list_elem_t *list_1, Optimizator_function_list_elem_t *list_2)
{
	bool ret = FALSE;
	Optimizator_function_list_elem_t *temp_1 = list_1;
	Optimizator_function_list_elem_t *temp_2 = list_2;

	if (get_list_size(temp_1) == get_list_size(temp_2))
	{
		int i;
		int size = get_list_size(temp_1);

		for (i = 0; i < size; i++)
		{
			if (temp_1->elem_type == temp_2->elem_type)
			{
				if (temp_1->elem_type == TE_COMPLEX)
				{
					if (list_cmp(temp_1->element.complex_var.complex_var_elements, temp_2->element.complex_var.complex_var_elements) == FALSE)
					{
						//if differs, exit the for loop
						break;
					}
				}
				else if (temp_1->elem_type == TE_OPERATOR)
				{
					if ((temp_1->element.operator.bracket != temp_2->element.operator.bracket) ||
						(temp_1->element.operator.operation != temp_2->element.operator.operation))
					{
						//if differs, exit the for loop
						break;
					}
				}
				else if (temp_1->elem_type == TE_VARIABLE)
				{
					if ((strcmp(temp_1->element.symple_var.type, temp_2->element.symple_var.type) != 0) ||
						(strcmp(temp_1->element.symple_var.value, temp_2->element.symple_var.value) != 0))
					{
						//if differs, exit the for loop
						break;
					}
				}

				temp_1 = temp_1->next;
				temp_2 = temp_2->next;
			}
			else
			{
				break;
			}
		}

		if (i == size)
		{
			//for loop finished, all elements are the same
			ret = TRUE;
		}
	}

	return ret;
}

/*
 *  Function: new_operator
 *  Description: Ade new operatot to list tail
 *  Parameters: operator - operator
 *              list_head - list to add element
 *  Returns: void
 */
void new_operator(Optimizator_operator_t operator, Optimizator_function_list_elem_t **list_head)
{
	Optimizator_function_list_elem_t *new_elem = malloc(sizeof(Optimizator_function_list_elem_t));
	Optimizator_function_list_elem_t *temp = *list_head;

	new_elem->elem_type = TE_OPERATOR;

	new_elem->element.operator.bracket = operator.bracket;
	new_elem->element.operator.operation = operator.operation;

	//find list tail
	if (*list_head == NULL)
	{
		new_elem->previous = NULL;
		*list_head = new_elem;
	}
	else
	{
		while (temp->next != NULL)
		{
			temp = temp->next;
		}

		temp->next = new_elem;
		new_elem->previous = temp;
	}

	new_elem->next = NULL;

}

/*
 *  Function: new_simple_element
 *  Description: Ade new simple element to list tail
 *  Parameters: policy - policy as string
 *              tokens - array of tokens from JSON file
 *              obj_idx - index of doc in tokens array
 *              list_head - list to add element
 *  Returns: void
 */
void new_simple_element(char *policy, jsmntok_t *tokens, int obj_idx, Optimizator_function_list_elem_t **list_head)
{
	Optimizator_function_list_elem_t* new_elem = malloc(sizeof(Optimizator_function_list_elem_t));
	Optimizator_function_list_elem_t* temp = *list_head;
	Optimizator_symbol_t new_symbol = 0;

	new_elem->elem_type = TE_VARIABLE;

	memcpy(new_elem->element.symple_var.type, &policy[tokens[obj_idx + 2].start],
		(tokens[obj_idx + 2].end - tokens[obj_idx + 2].start) > OPT_MAX_STR_LEN ?
			OPT_MAX_STR_LEN : (tokens[obj_idx + 2].end - tokens[obj_idx + 2].start));

	memcpy(new_elem->element.symple_var.value, &policy[tokens[obj_idx + 4].start],
		(tokens[obj_idx + 4].end - tokens[obj_idx + 4].start) > OPT_MAX_STR_LEN ?
			OPT_MAX_STR_LEN : (tokens[obj_idx + 4].end - tokens[obj_idx + 4].start));

	if (strncmp(&policy[tokens[obj_idx + 2].start], "boolean", strlen("boolean")) == 0)
	{
		if (strncmp(&policy[tokens[obj_idx + 4].start], "true", strlen("true")) == 0)
		{
			new_symbol = ONE;
		}
		else
		{
			new_symbol = ZERO;
		}
	}

	if (new_symbol == 0)
	{
		bool found = FALSE;

		//check if this element is already in the list
		while (temp)
		{
			if (temp->elem_type == TE_VARIABLE &&
				strcmp(new_elem->element.symple_var.type, temp->element.symple_var.type) == 0 &&
				strcmp(new_elem->element.symple_var.value, temp->element.symple_var.value) == 0)
			{
				found = TRUE;
				break;
			}
			else
			{
				temp = temp->next;
			}
		}

		if (found)
		{
			//if element is already in the list, new element must get the same symbol
			new_symbol = temp->element.symple_var.symbol;
		}
	}

	temp = *list_head;

	if (!temp)
	{
		//new element is head
		*list_head = new_elem;
		new_elem->previous = NULL;
		new_elem->next = NULL;

		if (new_symbol == 0)
		{
			if (*list_head == goc_head || *list_head == doc_head)
			{
				new_symbol = A;
			}
			else
			{
				//for complex variable list members, different symbols are used
				if (sub_var_symbol_cnt == 0)
				{
					new_symbol = a;
					sub_var_symbol_cnt = a;
				}
				else
				{
					sub_var_symbol_cnt++;
					new_symbol = sub_var_symbol_cnt;
				}
			}
		}
	}
	else
	{
		//get list tail
		while (temp->next != NULL)
		{
			temp = temp->next;
		}

		temp->next = new_elem;
		new_elem->previous = temp;
		new_elem->next = NULL;

		if (new_symbol == 0)
		{
			Optimizator_symbol_t top_symbol = 0;

			//find top symbol
			while (temp)
			{
				//search for last element which is not operator
				if (temp->elem_type != TE_OPERATOR)
				{
					if (temp->elem_type == TE_COMPLEX)
					{
						if (temp->element.complex_var.symbol > top_symbol)
						{
							top_symbol = temp->element.complex_var.symbol;
						}
					}
					else if (temp->elem_type == TE_VARIABLE)
					{
						if (temp->element.symple_var.symbol > top_symbol)
						{
							top_symbol = temp->element.symple_var.symbol;
						}
					}
				}

				temp = temp->previous;
			}

			if (top_symbol <= ONE)
			{
				//this is the first non boolean variable
				if (*list_head == goc_head || *list_head == doc_head)
				{
					new_symbol = A;
				}
				else
				{
					//for complex variable list members, different symbols are used
					if (sub_var_symbol_cnt == 0)
					{
						new_symbol = a;
						sub_var_symbol_cnt = a;
					}
					else
					{
						sub_var_symbol_cnt++;
						new_symbol = sub_var_symbol_cnt;
					}
				}
			}
			else
			{
				//assigne next available symbol
				if (*list_head == goc_head || *list_head == doc_head)
				{
					new_symbol = top_symbol + 1;
				}
				else
				{
					//for complex variable list members, different symbols are used
					sub_var_symbol_cnt++;
					new_symbol = sub_var_symbol_cnt;
				}
			}
		}
	}

	new_elem->element.symple_var.symbol = new_symbol;
}

/*
 *  Function: new_complex_element
 *  Description: Ade new complex element to list tail
 *  Parameters: policy - policy as string
 *              tokens - array of tokens from JSON file
 *              att_list_idx - index of attribute list in tokens array
 *              operation_idx - index of operation in tokens array
 *              list_head - list to add element
 *  Returns: void
 */
void new_complex_element(char *policy, jsmntok_t *tokens, int att_list_idx, int operation_idx, Optimizator_function_list_elem_t **list_head)
{
	bool found = FALSE;
	Optimizator_function_list_elem_t* new_elem = malloc(sizeof(Optimizator_function_list_elem_t));
	Optimizator_function_list_elem_t* temp = *list_head;
	Optimizator_symbol_t new_symbol = 0;
	Optimizator_operator_t operator;

	new_elem->elem_type = TE_COMPLEX;

	//create variable list

	int op_len = tokens[operation_idx + 1].end - tokens[operation_idx + 1].start;
	char op_c[op_len];
	memcpy(op_c, &policy[tokens[operation_idx + 1].start], op_len);
	Parser_operations_e op = Parser_get_op(op_c, op_len);

	//open bracket
	operator.bracket = BR_OPEN;
	operator.operation = OP_UNKNOWN;
	new_operator(operator, &new_elem->element.complex_var.complex_var_elements);

	//add each object from att list as new variable
	int temp_idx = att_list_idx + 2;

	while (temp_idx >= 0)
	{
		//TODO: consider the possibility that this might be another complex variable

		//simple variable
		new_simple_element(policy, tokens, temp_idx, &new_elem->element.complex_var.complex_var_elements);

		temp_idx = Parser_next_object_sibling_idx(tokens, temp_idx, Parser_end_of_current_idx(tokens, att_list_idx, tok_num));

		//add operation after every variable, except the last one
		if (temp_idx >= 0)
		{
			operator.bracket = BR_NONE;
			operator.operation = op;
			new_operator(operator, &new_elem->element.complex_var.complex_var_elements);
		}
	}

	// close brcket
	operator.bracket = BR_CLOSED;
	operator.operation = OP_UNKNOWN;
	new_operator(operator, &new_elem->element.complex_var.complex_var_elements);

	//check if this element is already in the list
	while (temp)
	{
		if (temp->elem_type == TE_COMPLEX)
		{
			if (list_cmp(temp->element.complex_var.complex_var_elements, new_elem->element.complex_var.complex_var_elements))
			{
				found = TRUE;
				break;
			}
		}

		temp = temp->next;
	}

	if (found)
	{
		//if element is already in the list, new element must get the same symbol
		new_symbol = temp->element.complex_var.symbol;
	}

	//add new element to the list
	temp = *list_head;

	if (!temp)
	{
		//new element is head
		*list_head = new_elem;
		new_elem->previous = NULL;
		new_elem->next = NULL;

		if (new_symbol == 0)
		{
			new_symbol = A;
		}
	}
	else
	{
		//get list tail
		while (temp->next != NULL)
		{
			temp = temp->next;
		}

		temp->next = new_elem;
		new_elem->previous = temp;
		new_elem->next = NULL;

		if (new_symbol == 0)
		{
			Optimizator_symbol_t top_symbol = 0;

			//find top symbol
			while (temp)
			{
				//search for last element which is not operator
				if (temp->elem_type != TE_OPERATOR)
				{
					if (temp->elem_type == TE_COMPLEX)
					{
						if (temp->element.complex_var.symbol > top_symbol)
						{
							top_symbol = temp->element.complex_var.symbol;
						}
					}
					else if (temp->elem_type == TE_VARIABLE)
					{
						if (temp->element.symple_var.symbol > top_symbol)
						{
							top_symbol = temp->element.symple_var.symbol;
						}
					}
				}

				temp = temp->previous;
			}

			if (top_symbol <= ONE)
			{
				new_symbol = A;
			}
			else
			{
				//assigne next available symbol
				new_symbol = top_symbol + 1;
			}
		}
	}

	new_elem->element.complex_var.symbol = new_symbol;
}

/*
 *  Function: obj_to_expression
 *  Description: Parse object, and fill expression list
 *  Parameters: policy - policy as string
 *              tokens - array of tokens from JSON file
 *              obj_idx - index of object in tokens array
 *              max_idx - array size
 * 				list_head - list to fill
 *  Returns: Number of elements added to list
 */
int obj_to_expression(char *policy, jsmntok_t *tokens, int obj_idx, int max_idx, Optimizator_function_list_elem_t **list_head)
{
	int ret = 0;
	int att_list_idx = -1;
	int operation_idx = -1;
	Optimizator_operator_t operator;

	if (strncmp(&policy[tokens[obj_idx + 1].start], "type", strlen("type")) == 0)
	{
		//simple variable
		new_simple_element(policy, tokens, obj_idx, list_head);

		ret++;
		return ret;
	}
	else if (strncmp(&policy[tokens[obj_idx + 1].start], "attribute_list", strlen("attribute_list")) == 0)
	{
		att_list_idx = obj_idx + 1;
		operation_idx = Parser_next_key_sibling_idx(tokens, att_list_idx, max_idx);
	}
	else if (strncmp(&policy[tokens[obj_idx + 1].start], "operation", strlen("operation")) == 0)
	{
		operation_idx = obj_idx + 1;
		att_list_idx = Parser_next_key_sibling_idx(tokens, operation_idx, max_idx);
	}

	int op_len = tokens[operation_idx + 1].end - tokens[operation_idx + 1].start;
	char op_c[op_len];
	memcpy(op_c, &policy[tokens[operation_idx + 1].start], op_len);
	Parser_operations_e op = Parser_get_op(op_c, op_len);

	if (op == OP_AND || op == OP_OR)
	{
		//open bracket
		operator.bracket = BR_OPEN;
		operator.operation = OP_UNKNOWN;
		new_operator(operator, list_head);
		ret++;

		//add each object from att list as new variable
		int temp_idx = att_list_idx + 2;

		while (temp_idx >= 0)
		{
			ret += obj_to_expression(policy, tokens, temp_idx, Parser_end_of_current_idx(tokens, att_list_idx, max_idx), list_head);

			temp_idx = Parser_next_object_sibling_idx(tokens, temp_idx, Parser_end_of_current_idx(tokens, att_list_idx, max_idx));

			//add operation after every variable, except the last one
			if (temp_idx >= 0)
			{
				operator.bracket = BR_NONE;
				operator.operation = op;
				new_operator(operator, list_head);
				ret++;
			}
		}

		// close brcket
		operator.bracket = BR_CLOSED;
		operator.operation = OP_UNKNOWN;
		new_operator(operator, list_head);
		ret++;
	}
	else
	{
		//new complex element
		new_complex_element(policy, tokens, att_list_idx, operation_idx, list_head);
		ret++;
	}

	return ret;
}

/*
 *  Function: sprint_expression
 *  Description: Print expression from list
 *  Parameters: list_head - list to print from
 *              print_sub_var - should sub-variables be print
 *              buffer - buffer to fill
 *  Returns: written characters num
 */
int sprint_expression(Optimizator_function_list_elem_t *list_head, bool print_sub_var, char *buffer)
{
	int written = 0;
	Optimizator_function_list_elem_t *element = list_head;

	if (list_head == NULL || buffer == NULL)
	{
		return 0;
	}

	if (list_head == goc_head || list_head == doc_head)
	{
		written += sprintf(buffer, "F = ");
	}

	while (element && (written <= OPTIMIZATOR_MAX_BUFFER - OPTIMIZATOR_CHAR_PER_ITERATION)) //we must not write to memory over buffer limit
	{
		switch (element->elem_type)
		{
			case TE_COMPLEX:
				if (print_sub_var)
				{
					written += sprint_expression(element->element.complex_var.complex_var_elements, TRUE, &buffer[written]);
				}
				else
				{
					written += sprintf(&buffer[written], "%c", element->element.complex_var.symbol);
				}

				break;
			case TE_VARIABLE:
				written += sprintf(&buffer[written], "%c", element->element.symple_var.symbol);
				break;
			case TE_OPERATOR:
				switch (element->element.operator.operation)
				{
					//TODO: add others
					case OP_AND:
						written += sprintf(&buffer[written], " * ");
						break;
					case OP_OR:
						written += sprintf(&buffer[written], " + ");
						break;
					case OP_EQ:
						written += sprintf(&buffer[written], " = ");
						break;
					case OP_GEQ:
						written += sprintf(&buffer[written], " >= ");
						break;
					case OP_LEQ:
						written += sprintf(&buffer[written], " <= ");
						break;
					case OP_GTE:
						written += sprintf(&buffer[written], " > ");
						break;
					case OP_LTE:
						written += sprintf(&buffer[written], " < ");
						break;
					case OP_UNKNOWN:
						if (element->element.operator.bracket == BR_OPEN)
						{
							written += sprintf(&buffer[written], "(");
						}
						else if (element->element.operator.bracket == BR_CLOSED)
						{
							written += sprintf(&buffer[written], ")");
						}
						break;
					default:
						written += sprintf(&buffer[written], " # ");
						break;
				}
			default:
				break;
		}

		element = element->next;
	}

	return written;
}

/*
 *  Function: sprint_object
 *  Description: Print json object from list
 *  Parameters: list_head - list to print from
 *              buffer - buffer to fill
 *              indentation - indentation starting point
 *  Returns: written characters num
 */
int sprint_object(Optimizator_function_list_elem_t *list_head, char *buffer, int indentation)
{
	bool is_single_element = FALSE;
	char operation_c[OPTIMIZATIR_MAX_OPERATION_LEN];
	int written = 0;
	int indent_cnt = indentation;
	Optimizator_function_list_elem_t *element = list_head;

	if (list_head == NULL || buffer == NULL)
	{
		return 0;
	}

	written += sprintf(&buffer[written], "{\n");
	indent_cnt++;

	for (int i = 0; i < indent_cnt; i++)
	{
		written += sprintf(&buffer[written], "\t");
	}

	while (element)
	{
		if (element->previous == NULL &&
			element->next == NULL)
		{
			is_single_element = TRUE;
		}


		switch (element->elem_type)
		{
			case TE_OPERATOR:
				if (element->element.operator.bracket == BR_OPEN)
				{
					written += sprintf(&buffer[written], "\"attribute_list\":\n");
					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
					written += sprintf(&buffer[written], "[\n");

					indent_cnt++;
					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
				}
				else if (element->element.operator.bracket == BR_CLOSED)
				{
					written += sprintf(&buffer[written], "],\n");
					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
					written += sprintf(&buffer[written], "\"operation\":\"%s\"\n", operation_c);

					indent_cnt--;
					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
				}
				else
				{
					memset(operation_c, 0, OPTIMIZATIR_MAX_OPERATION_LEN * sizeof(char));

					switch (element->element.operator.operation)
					{
						case OP_AND:
							memcpy(operation_c, "and", strlen("and"));
							break;
						case OP_OR:
							memcpy(operation_c, "or", strlen("or"));
							break;
						case OP_EQ:
							memcpy(operation_c, "eq", strlen("eq"));
							break;
						case OP_LEQ:
							memcpy(operation_c, "leq", strlen("leq"));
							break;
						case OP_GEQ:
							memcpy(operation_c, "geq", strlen("geq"));
							break;
						case OP_LTE:
							memcpy(operation_c, "lte", strlen("lte"));
							break;
						case OP_GTE:
							memcpy(operation_c, "gte", strlen("gte"));
							break;
						default:
							break;
					}
				}
				break;
			case TE_VARIABLE:
				if (!is_single_element)
				{
					written += sprintf(&buffer[written], "{\n");
					indent_cnt++;

					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
				}

				written += sprintf(&buffer[written], "\"type\":\"%s\",\n", element->element.symple_var.type);

				for (int i = 0; i < indent_cnt; i++)
				{
					written += sprintf(&buffer[written], "\t");
				}

				written += sprintf(&buffer[written], "\"value\":\"%s\"\n", element->element.symple_var.value);

				indent_cnt--;

				for (int i = 0; i < indent_cnt; i++)
				{
					written += sprintf(&buffer[written], "\t");
				}

				if (!is_single_element)
				{
					written += sprintf(&buffer[written], "}");

					//if it's last element return indentation, else, add "," after "}"
					if (element->next->elem_type == TE_OPERATOR &&
						element->next->element.operator.bracket == BR_CLOSED)
					{
						written += sprintf(&buffer[written], "\n");
						indent_cnt--;
					}
					else
					{
						written += sprintf(&buffer[written], ",\n");
					}

					for (int i = 0; i < indent_cnt; i++)
					{
						written += sprintf(&buffer[written], "\t");
					}
				}
				break;
			case TE_COMPLEX:
				written += sprint_object(element->element.complex_var.complex_var_elements, &buffer[written], indent_cnt);

				//if it's last element, delete "," on the end and return one tab additionaly
				if (element->next->elem_type == TE_OPERATOR &&
					element->next->element.operator.bracket == BR_CLOSED)
				{
					for (int i = written; i > 0; i--)
					{
						if (buffer[i] == ',')
						{
							buffer[i] = ' ';
							break;
						}
					}

					buffer[written - 1] = '\0';
					written--;
					indent_cnt--;
				}
				break;
			default:
				break;
		}

		element = element->next;
	}

	written += sprintf(&buffer[written], "},\n");

	indent_cnt = indentation;
	for (int i = 0; i < indent_cnt; i++)
	{
		written += sprintf(&buffer[written], "\t");
	}

	return written;
}

/*
 *  Function: remove_dummy_brackets
 *  Description: Check if there are dummy brackets and
 *               remove them. E.g. z + (x) + y
 *  Parameters: expression - list to check
 *  Returns: void
 */
void remove_dummy_brackets(Optimizator_function_list_elem_t **expression)
{
	Optimizator_function_list_elem_t *temp = *expression;

	if (*expression == NULL)
	{
		return;
	}

	//loop through list
	while (temp)
	{
		//search for open bracket
		if (temp->elem_type == TE_OPERATOR &&
			temp->element.operator.bracket == BR_OPEN)
		{
			//if there is jus one element before closed bracket, delete both brackets
			if (temp->next->next->elem_type == TE_OPERATOR &&
				temp->next->next->element.operator.bracket == BR_CLOSED)
			{
				Optimizator_function_list_elem_t *elem_for_removal = temp;
				temp = elem_for_removal->next->next;

				if (elem_for_removal->previous != NULL)
				{
					elem_for_removal->previous->next = elem_for_removal->next;
				}

				if (elem_for_removal->next != NULL)
				{
					elem_for_removal->next->previous = elem_for_removal->previous;
				}

				free(elem_for_removal);

				elem_for_removal = temp;
				temp = elem_for_removal->next;

				if (elem_for_removal->previous != NULL)
				{
					elem_for_removal->previous->next = elem_for_removal->next;
				}

				if (elem_for_removal->next != NULL)
				{
					elem_for_removal->next->previous = elem_for_removal->previous;
				}

				free(elem_for_removal);

				continue;
			}
		}

		temp = temp->next;
	}
}

/*
 *  Function: check_absorption
 *  Description: If it's possible, perform logic absorption
 *  Parameters: expression - list to be optimized
 *  Returns: OA_NONE - no action taken
 *           OA_ABSORPTION - absorption performed
 */
Optimizator_optimization_actions_e check_absorption(Optimizator_function_list_elem_t **expression)
{
	bool remove_bracket = FALSE;
	Optimizator_optimization_actions_e ret = OA_NONE;
	Optimizator_function_list_elem_t *temp = *expression;

	if (*expression == NULL)
	{
		return ret;
	}

	//loop through list
	while (temp)
	{
		//search for open bracket
		if (temp->elem_type == TE_OPERATOR &&
			temp->element.operator.bracket == BR_OPEN)
		{
			Optimizator_function_list_elem_t *bracket = temp;
			Optimizator_function_list_elem_t *bracket_opn = bracket;
			Optimizator_function_list_elem_t *bracket_cls = NULL;

			/* Absorption law applies if there are two variables
			inside the brackets. We must count the elements before bracket
			is closed. */
			int el_cnt = 1;
			do
			{
				el_cnt++;
				bracket = bracket->next;
				if (bracket != NULL &&
					bracket->elem_type == TE_OPERATOR &&
					bracket->element.operator.bracket != BR_NONE)
				{
					break;
				}
			} while (bracket);

			if (bracket == NULL)
			{
				return ret;
			}

			if (bracket->element.operator.bracket == BR_OPEN)
			{
				//if set of breckets inside of breckets in found, check them first
				temp = bracket;
				continue;
			}
			else
			{
				bracket_cls = bracket;

				if (el_cnt == OPTIMIZATOR_REMOVAL_BRACKET_ELEMENTS - 1)
				{
					//variables inside of the brackets must differ
					Optimizator_symbol_t sym_1;
					if (bracket_opn->next->elem_type == TE_VARIABLE)
					{
						sym_1 = bracket_opn->next->element.symple_var.symbol;
					}
					else
					{
						sym_1 = bracket_opn->next->element.complex_var.symbol;
					}

					Optimizator_symbol_t sym_2;
					if (bracket_cls->previous->elem_type == TE_VARIABLE)
					{
						sym_2 = bracket_cls->previous->element.symple_var.symbol;
					}
					else
					{
						sym_2 = bracket_cls->previous->element.complex_var.symbol;
					}

					if (sym_1 != sym_2)
					{
						//operations inside and outside of brackets must differ
						Parser_operations_e inside_op = bracket_cls->previous->previous->element.operator.operation;
						Parser_operations_e before_op = OP_UNKNOWN;
						Parser_operations_e after_op = OP_UNKNOWN;
						Parser_operations_e outside_op = OP_UNKNOWN;

						if (bracket_opn->previous != NULL &&
							bracket_opn->previous->element.operator.bracket == BR_NONE) //eliminate bracket next to bracket case
						{
							before_op = bracket_opn->previous->element.operator.operation;
						}

						if (bracket_cls->next != NULL &&
							bracket_cls->next->element.operator.bracket == BR_NONE) //eliminate bracket next to bracket case
						{
							after_op = bracket_cls->next->element.operator.operation;
						}

						if (before_op == after_op)
						{
							outside_op = before_op;
						}
						else if (before_op == OP_UNKNOWN && after_op != OP_UNKNOWN)
						{
							outside_op = after_op;
						}
						else if (before_op != OP_UNKNOWN && after_op == OP_UNKNOWN)
						{
							outside_op = before_op;
						}
						else if (before_op != inside_op)
						{
							outside_op = before_op;
						}
						else if (after_op != inside_op)
						{
							outside_op = after_op;
						}

						if (inside_op != outside_op && outside_op != OP_UNKNOWN)
						{
							/* Check the each of the elements with commutative property with the bracket.
							E.g. F = a + b * c * (d + f) * g * (h + i) * j + k - for (d + f) bracket
							elements b, c, g, j will be checked. Same elements will be checked for (h + i) bracket,
							but that's another iteration. If any of these elements is also presented in the
							bracket (maximum once), bracket shall be absorped. */
							Optimizator_function_list_elem_t *absorper;

							if (outside_op == before_op)
							{
								absorper = bracket_opn->previous;

								while (absorper)
								{
									if (absorper->elem_type == TE_VARIABLE)
									{
										if (absorper->element.symple_var.symbol == sym_1 ||
											absorper->element.symple_var.symbol == sym_2)
										{
											remove_bracket = TRUE;
											break;
										}
									}
									else if (absorper->elem_type == TE_COMPLEX)
									{
										if (absorper->element.complex_var.symbol == sym_1 ||
											absorper->element.complex_var.symbol == sym_2)
										{
											remove_bracket = TRUE;
											break;
										}
									}
									else if (absorper->elem_type == TE_OPERATOR)
									{
										if (absorper->element.operator.bracket == BR_NONE &&
											absorper->element.operator.operation != outside_op)
										{
											//break the loop when hit the operator different that outside_op
											break;
										}
									}

									absorper = absorper->previous;
								}
							}

							if (outside_op == after_op)
							{
								absorper = bracket_cls->next;

								while (absorper)
								{
									if (absorper->elem_type == TE_VARIABLE)
									{
										if (absorper->element.symple_var.symbol == sym_1 ||
											absorper->element.symple_var.symbol == sym_2)
										{
											remove_bracket = TRUE;
											break;
										}
									}
									else if (absorper->elem_type == TE_COMPLEX)
									{
										if (absorper->element.complex_var.symbol == sym_1 ||
											absorper->element.complex_var.symbol == sym_2)
										{
											remove_bracket = TRUE;
											break;
										}
									}
									else if (absorper->elem_type == TE_OPERATOR)
									{
										if (absorper->element.operator.bracket == BR_NONE &&
											absorper->element.operator.operation != outside_op)
										{
											//break the loop when hit the operator different that outside_op
											break;
										}
									}

									absorper = absorper->next;
								}
							}

							if (remove_bracket)
							{
								Optimizator_function_list_elem_t *elem_for_removal;

								if (outside_op == before_op)
								{
									//start from before element, till the close bracket
									elem_for_removal = bracket_opn->previous;
								}
								else
								{
									//start from the open bracket, till the element after
									elem_for_removal = bracket_opn;
								}

								for (int i = 0; i < OPTIMIZATOR_REMOVAL_BRACKET_ELEMENTS; i++)
								{
									if (elem_for_removal->previous != NULL)
									{
										elem_for_removal->previous->next = elem_for_removal->next;
									}

									if (elem_for_removal->next != NULL)
									{
										elem_for_removal->next->previous = elem_for_removal->previous;
									}


									if (elem_for_removal->elem_type == TE_COMPLEX)
									{
										//free sub-list
										while (elem_for_removal->element.complex_var.complex_var_elements)
										{
											Optimizator_function_list_elem_t *sub_elem_for_removal = elem_for_removal->element.complex_var.complex_var_elements;
											elem_for_removal->element.complex_var.complex_var_elements = sub_elem_for_removal->next;

											free(sub_elem_for_removal);
										}
									}

									Optimizator_function_list_elem_t *helper = elem_for_removal;
									elem_for_removal = helper->next;

									free(helper);
								}

								ret = OA_ABSORPTION;
								temp = elem_for_removal;
								continue;
							}
						}
					}
				}
			}
		}

		temp = temp->next;
	}

	/* If absorption is performed, we might want to check in another iteration,
	whether there is now condition for additional absorptions, due to changes in expression. */
	if (ret == OA_ABSORPTION)
	{
		check_absorption(expression);
	}

	return ret;
}

/*
 *  Function: optimize_expression
 *  Description: Takes expression list and performes optimization, or
 *               do nothing if there is nothing to be optimized
 *  Parameters: expression - list to be optimized
 *  Returns: RE_ERROR - Error occurred
 *           RE_NO_ACTION - Nothing to be optimized
 *           RE_SUCCESS - Optimization done
 */
Optimizator_return_e optimize_expression(Optimizator_function_list_elem_t **expression)
{
	Optimizator_return_e ret = RE_ERROR;

	if (*expression == NULL)
	{
		return ret;
	}

	if (check_absorption(expression) == OA_NONE)
	{
		ret = RE_NO_ACTION;
	}
	else
	{
		remove_dummy_brackets(expression);
		ret = RE_SUCCESS;
	}

	return ret;
}

/*
 *  Function: expression_to_policy
 *  Description: Create json file from optimizes expression
 *  Parameters: policy - policy to check/optimze
 *              tokens - array of tokens from JSON file
 *              new_json_path - location where optimized .json file will
 *                              be created
 *  Returns: void
 */
void expression_to_policy(char* policy, jsmntok_t *tokens, char *new_json_path)
{
	char new_path[OPT_MAX_STR_LEN];
	char buffer[OPTIMIZATOR_MAX_BUFFER];
	int tok_idx = 0;
	int doc_start = 0;
	int doc_end = 0;
	int goc_start = 0;
	int goc_end = 0;
	int before = 0;
	int after = 0;
	int wrote = 0;

	if (policy == NULL || tokens == NULL || new_json_path == NULL)
	{
		return;
	}

	sprintf(new_path, "%s/optimized.json", new_json_path);

	FILE *file = fopen(new_path, "w+");

	//find DoC and GoC
	while (tok_idx < tok_num)
	{
		if (strncmp(&policy[tokens[tok_idx].start], "policy_doc", strlen("policy_doc")) == 0)
		{
			doc_start = tokens[tok_idx + 1].start;
			doc_end = tokens[tok_idx + 1].end;
		}
		else if (strncmp(&policy[tokens[tok_idx].start], "policy_goc", strlen("policy_goc")) == 0)
		{
			goc_start = tokens[tok_idx + 1].start;
			goc_end = tokens[tok_idx + 1].end;
		}

		tok_idx++;
	}

	(doc_start < goc_start) ? (before = doc_start) : (before = goc_start);
	(doc_end > goc_end) ? (after = doc_end) : (after = goc_end);

	//copy everything from policy before doc/goc
	fwrite(policy, 1, before, file);

	if (before == doc_start)
	{
		memset(buffer, 0, OPT_MAX_STR_LEN * sizeof(char));
		wrote = sprint_object(doc_head, buffer, OPTIMIZATOR_DOC_GOC_INDENTATION);
		fseek(file, 0, SEEK_END);
		fwrite(buffer, 1, wrote, file);

		fseek(file, 0, SEEK_END);
		fwrite("\"policy_goc\":", 1, strlen("\"policy_goc\":"), file);

		memset(buffer, 0, OPT_MAX_STR_LEN * sizeof(char));
		wrote = sprint_object(goc_head, buffer, OPTIMIZATOR_DOC_GOC_INDENTATION);
		fseek(file, 0, SEEK_END);
		fwrite(buffer, 1, wrote, file);
	}
	else
	{
		memset(buffer, 0, OPT_MAX_STR_LEN * sizeof(char));
		wrote = sprint_object(goc_head, buffer, OPTIMIZATOR_DOC_GOC_INDENTATION);
		fseek(file, 0, SEEK_END);
		fwrite(buffer, 1, wrote, file);

		fseek(file, 0, SEEK_END);
		fwrite("\"policy_doc\":", 1, strlen("\"policy_doc\":"), file);

		memset(buffer, 0, OPT_MAX_STR_LEN * sizeof(char));
		wrote = sprint_object(doc_head, buffer, OPTIMIZATOR_DOC_GOC_INDENTATION);
		fseek(file, 0, SEEK_END);
		fwrite(buffer, 1, wrote, file);
	}

	/* If first non-whitespace character after doc/goc is "}", it means that
	policy_object is done and "," after doc/goc should be deleted. */
	for (int i = after; i < strlen(policy); i++)
	{
		if (policy[i] >= OPTIMIZATOR_FIRST_NON_WHITESPACE_ASCII_CHAR)
		{
			if (policy[i] == '}')
			{
				char ch;
				int cnt = 0;
				fseek(file, 0, SEEK_END);
				ch = fgetc(file);

				while (ch != ',')
				{
					fseek(file, 0 - cnt, SEEK_END);
					ch = fgetc(file);
					cnt++;
				}

				fseek(file, 1 - cnt, SEEK_END);
				fputc(OPTIMIZATOR_ASCII_SPACE, file);
			}

			break;
		}
	}

	//copy everything from policy after doc/goc
	fseek(file, 0, SEEK_END);
	fwrite(&policy[after], 1, strlen(policy) - after, file);

	fclose(file);
}

/*
 *  Function: Optimizator_optimize_pol
 *  Description: Check if logical operations in policy can be optimized and
 *               create new, optimized, .json file
 *  Parameters: policy - policy to check/optimze
 *              new_json_path - location where optimized .json file will
 *                              be created
 *  Returns: RE_ERROR - Error occurred
 *           RE_NO_ACTION - Nothing to be optimized
 *           RE_SUCCESS - Optimization done
 */
Optimizator_return_e Optimizator_optimize_pol(char* policy, char* new_json_path)
{
	jsmn_parser parser;
	jsmntok_t tokens[OPTIMIZATOR_MAX_TOKENS];
	Optimizator_return_e ret_doc = RE_ERROR;
	Optimizator_return_e ret_goc = RE_ERROR;

	if (policy == NULL || new_json_path == NULL)
	{
		printf("\n\nERROR[%s] - Bad input parameter.\n\n", __FUNCTION__);
		return RE_ERROR;
	}

    jsmn_init(&parser);
    tok_num = jsmn_parse(&parser, policy, strlen(policy), tokens, OPTIMIZATOR_MAX_TOKENS);

	if (tok_num <= 0)
	{
		printf("\n\nERROR[%s] - JSON parsing failed.\n\n", __FUNCTION__);
		return RE_ERROR;
	}

	for (int i = 0; i < tok_num; i++)
	{
		if (strncmp(&policy[tokens[i].start], "policy_doc", strlen("policy_doc")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i + 1].type == JSMN_OBJECT)
		{
			obj_to_expression(policy, tokens, i + 1, Parser_end_of_current_idx(tokens, i, tok_num), &doc_head);
		}
		else if (strncmp(&policy[tokens[i].start], "policy_goc", strlen("policy_goc")) == 0 &&
                    tokens[i].size == 1 &&
                    tokens[i + 1].type == JSMN_OBJECT)
		{
			obj_to_expression(policy, tokens, i + 1, Parser_end_of_current_idx(tokens, i, tok_num), &goc_head);
		}

		//TODO: Add support for obligation circuits
	}

	ret_doc = optimize_expression(&doc_head);
	ret_goc = optimize_expression(&goc_head);

	if (ret_doc == RE_ERROR || ret_goc == RE_ERROR)
	{
		printf("\n\nERROR: Optimization failed\n\n");
	}
	else if (ret_doc == RE_NO_ACTION && ret_goc == RE_NO_ACTION)
	{
		printf("\n\nNOTHING TO OPTIMIZE\n\n");
	}
	else
	{
		expression_to_policy(policy, tokens, new_json_path);
	}

	char doc_buffer[OPTIMIZATOR_MAX_BUFFER];
	sprint_expression(doc_head, FALSE, doc_buffer);
	printf("\n\nDoC Function:\n%s\n\n", doc_buffer);

	char goc_buffer[OPTIMIZATOR_MAX_BUFFER];
	sprint_expression(goc_head, FALSE, goc_buffer);
	printf("\n\nGoC Function:\n%s\n\n", goc_buffer);

	//free the list
	while (doc_head)
	{
		Optimizator_function_list_elem_t *temp = doc_head;

		doc_head = temp->next;

		if (temp->elem_type == TE_COMPLEX)
		{
			//free sublist
			while (temp->element.complex_var.complex_var_elements)
			{
				Optimizator_function_list_elem_t *sub_temp = temp->element.complex_var.complex_var_elements;

				temp->element.complex_var.complex_var_elements = sub_temp->next;

				free(sub_temp);
			}
		}

		free(temp);
	}

	//free the list
	while (goc_head)
	{
		Optimizator_function_list_elem_t *temp = goc_head;

		goc_head = temp->next;

		if (temp->elem_type == TE_COMPLEX)
		{
			//free sublist
			while (temp->element.complex_var.complex_var_elements)
			{
				Optimizator_function_list_elem_t *sub_temp = temp->element.complex_var.complex_var_elements;

				temp->element.complex_var.complex_var_elements = sub_temp->next;

				free(sub_temp);
			}
		}

		free(temp);
	}

	return RE_SUCCESS;
}
