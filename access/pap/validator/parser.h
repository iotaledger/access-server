/***************************************************************************
 * Project:     Remote Access Delegation
 * 
 * Module:      Policy validation tool
 * 
 * Info:        This module is created in order to validate or
 *              discard policy, also it has functionality to 
 *              perform policy optimisation.
 * 
 * File:        parser.h
 * 
 * Designed-by: Strahinja Golic
 * 
 * History:     25.03.2020. - Initial version
 *****************************************************************************/
#ifndef __PARSER_H__
#define __PARSER_H__

/***************************************************************************
 * INCLUDES
****************************************************************************/

#define JSMN_HEADER
#include "jsmn.h"

/***************************************************************************
 * ENUMERATIONS
****************************************************************************/

typedef enum
{
    OP_EQ,
    OP_LEQ,
    OP_GEQ,
    OP_LTE,
    OP_GTE,
	OP_AND,
	OP_OR,
    OP_UNKNOWN
} Parser_operations_e;

/***************************************************************************
 * FUNCTION DECLARATIONS
****************************************************************************/

int Parser_next_key_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int Parser_next_object_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int Parser_end_of_current_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int Parser_object_parent_idx(jsmntok_t* _tokens, int cur_idx);
Parser_operations_e Parser_get_op(char *str, int len);

#endif