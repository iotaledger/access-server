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
 * File:        optimizator.h
 *
 * Designed-by: Strahinja Golic
 *
 * History:     25.03.2020. - Initial version
 *****************************************************************************/
#ifndef __OPTIMIZATOR_H__
#define __OPTIMIZATOR_H__

/***************************************************************************
 * INCLUDES
****************************************************************************/
#include "symbols.h"
#include "parser.h"

/***************************************************************************
 * DEFINES
****************************************************************************/
#define DEBUG {printf("\n\n%s:%d\n\n",__FUNCTION__,__LINE__);}
#define OPT_MAX_STR_LEN 256
#define TRUE 1
#define FALSE 0

/***************************************************************************
 * ENUMERATIONS
****************************************************************************/
typedef enum
{
    RE_ERROR,
    RE_NO_ACTION,
    RE_SUCCESS
} Optimizator_return_e;

typedef enum
{
    TE_OPERATOR,
    TE_VARIABLE,
    TE_COMPLEX
} Optimizator_type_of_func_elem_e;

typedef enum
{
    BR_OPEN,
    BR_CLOSED,
    BR_NONE
} Optimizator_bracket_e;

typedef enum
{
    OA_NONE = 0x00,
    OA_ABSORPTION = 0x01
} Optimizator_optimization_actions_e;

/***************************************************************************
 * STRUCTURES
****************************************************************************/
typedef _Bool bool;
typedef int Optimizator_symbol_t;
typedef struct _function_list Optimizator_function_list_elem_t;

typedef struct
{
    Optimizator_bracket_e bracket;
    Parser_operations_e operation;
} Optimizator_operator_t;

typedef struct
{
    char type[OPT_MAX_STR_LEN];
    char value[OPT_MAX_STR_LEN];
    Optimizator_symbol_t symbol;
} Optimizator_log_var_t;

typedef struct
{
    Optimizator_function_list_elem_t *complex_var_elements;
    Optimizator_symbol_t symbol;
} Optimizator_complex_var_t;


typedef union
{
    Optimizator_operator_t operator;
    Optimizator_log_var_t symple_var;
    Optimizator_complex_var_t complex_var;
} Optimizator_function_element_t;


struct _function_list
{
    struct _function_list *previous;
    struct _function_list *next;
    Optimizator_type_of_func_elem_e elem_type;
    Optimizator_function_element_t element;
};


/***************************************************************************
 * FUNCTION DECLARATIONS
****************************************************************************/
Optimizator_return_e Optimizator_optimize_pol(char*, char*);
#endif
