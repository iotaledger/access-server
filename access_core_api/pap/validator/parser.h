/*
 * This file is part of the IOTA Access distribution
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

/***************************************************************************
 * Project:     IOTA Access
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

typedef enum { OP_EQ, OP_LEQ, OP_GEQ, OP_LTE, OP_GTE, OP_AND, OP_OR, OP_UNKNOWN } parser_operations_e;

/***************************************************************************
 * FUNCTION DECLARATIONS
 ****************************************************************************/

int parser_next_key_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int parser_next_object_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int parser_end_of_current_idx(jsmntok_t* _tokens, int cur_idx, int max_idx);
int parser_object_parent_idx(jsmntok_t* _tokens, int cur_idx);
parser_operations_e parser_get_op(char* str, int len);

#endif
