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
 * File:        validator.c
 *
 * Designed-by: Djordje Golubovic
 *
 * History:     21.02.2020. - Initial version
 *              22.03.2020. - Value limitations check
 *              25.03.2020. - Moved parser functions to helper module
 *****************************************************************************/

/***************************************************************************
 * INCLUDES
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "validator.h"
#include "parser.h"

#define JSMN_HEADER
#include "jsmn.h"

/***************************************************************************
 * DEFINES
****************************************************************************/

#define COST_STR_SIZE 3
#define POL_OBJC_MIN_MEMBERS 2

/***************************************************************************
 * GLOBAL VARIABLES
****************************************************************************/

/* After new functions are supported, this list should be expanded with them,
including delimiter "|" after each name of the function. */
static char Validator_supported_hash_fns[] =
    "sha-256|";

static int n = 0; // number of tokens

/***************************************************************************
 * FUNCTION IMPLEMENTATIONS
****************************************************************************/

/*
 *  Function: check_eq
 *  Description: Check if params are equal. In case when time or hash function
 *               needs to be checked, only first parameter can be sent,
 *               function will get the current time (if val2 is NULL) or list
 *               of supported hash functions.
 *  Parameters: val1 - first value to check
 *              val2 - second value to check
 *              type - type of params
 *  Returns: CR_TRUE - equale
 *           CR_FALSE - not equal
 *           CR_NOT_SUPPORTED - not supported type
 *           CR_BAD_ARG - bad argument
 */
Validator_check_res_e check_eq(void *val1, void *val2, Validator_cmp_types_e type)
{
    char *tok;
    int *int_val_1;
    int *int_val_2;
    float *float_val_1;
    float *float_val_2;
    unsigned long *time_val1;
    unsigned long *time_val2;

    if (val1 == NULL)
    {
        return CR_BAD_ARG;
    }

    switch (type)
    {
        case CT_INT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            int_val_1 = (int*)val1;
            int_val_2 = (int*)val2;

            return *int_val_1 == *int_val_2 ? CR_TRUE : CR_FALSE;

        case CT_FLOAT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            float_val_1 = (float*)val1;
            float_val_2 = (float*)val2;

            return *float_val_1 == *float_val_2 ? CR_TRUE : CR_FALSE;

        case CT_HASH_FN:
            tok = strtok(Validator_supported_hash_fns, "|");
            while (tok)
            {
                if ((strlen(tok) == strlen((char*)val1)) && (strncmp(tok, (char*)val1, strlen(tok)) == 0))
                {
                    return CR_TRUE;
                }
                else
                {
                    tok = strtok(NULL, "|");
                }
            }

            return CR_FALSE;

        case CT_TIME:
            time_val1 = (unsigned long*)val1;

            if (val2 == NULL)
            {
                unsigned long epoche = (unsigned long)time(NULL);
                time_val2 = &epoche;
            }
            else
            {
                time_val2 = (unsigned long*)val2;
            }

            return *time_val1 == *time_val2 ? CR_TRUE : CR_FALSE;

        case CT_ID:
            // TODO: What needs to be checked for ID case exactly?
        default:
            return CR_NOT_SUPPORTED;
    }
}

/*
 *  Function: check_leq
 *  Description: Check if first param is less or equal than second. In case
 *               when time needs to be checked, only first parameter can be
 *               sent, function will get the current time (if val2 is NULL).
 *  Parameters: val1 - first value to check
 *              val2 - second value to check
 *              type - type of params
 *  Returns: CR_TRUE - less or equale
 *           CR_FALSE - greater
 *           CR_NOT_SUPPORTED - not supported type
 *           CR_BAD_ARG - bad argument
 */
Validator_check_res_e check_leq(void *val1, void *val2, Validator_cmp_types_e type)
{
    char *tok;
    int *int_val_1;
    int *int_val_2;
    float *float_val_1;
    float *float_val_2;
    unsigned long *time_val1;
    unsigned long *time_val2;

    if (val1 == NULL)
    {
        return CR_BAD_ARG;
    }

    switch (type)
    {
        case CT_INT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            int_val_1 = (int*)val1;
            int_val_2 = (int*)val2;

            return *int_val_1 <= *int_val_2 ? CR_TRUE : CR_FALSE;

        case CT_FLOAT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            float_val_1 = (float*)val1;
            float_val_2 = (float*)val2;

            return *float_val_1 <= *float_val_2 ? CR_TRUE : CR_FALSE;

        case CT_TIME:
            time_val1 = (unsigned long*)val1;

            if (val2 == NULL)
            {
                unsigned long epoche = (unsigned long)time(NULL);
                time_val2 = &epoche;
            }
            else
            {
                time_val2 = (unsigned long*)val2;
            }

            return *time_val1 == *time_val2 ? CR_TRUE : CR_FALSE;

        case CT_HASH_FN:
        case CT_ID:
        default:
            return CR_NOT_SUPPORTED;
    }
}

/*
 *  Function: check_geq
 *  Description: Check if first param is greater or equal than second. In case
 *               when time needs to be checked, only first parameter can be
 *               sent, function will get the current time (if val2 is NULL).
 *  Parameters: val1 - first value to check
 *              val2 - second value to check
 *              type - type of params
 *  Returns: CR_TRUE - greater or equale
 *           CR_FALSE - less
 *           CR_NOT_SUPPORTED - not supported type
 *           CR_BAD_ARG - bad argument
 */
Validator_check_res_e check_geq(void *val1, void *val2, Validator_cmp_types_e type)
{
    char *tok;
    int *int_val_1;
    int *int_val_2;
    float *float_val_1;
    float *float_val_2;
    unsigned long *time_val1;
    unsigned long *time_val2;

    if (val1 == NULL)
    {
        return CR_BAD_ARG;
    }

    switch (type)
    {
        case CT_INT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            int_val_1 = (int*)val1;
            int_val_2 = (int*)val2;

            return *int_val_1 >= *int_val_2 ? CR_TRUE : CR_FALSE;

        case CT_FLOAT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            float_val_1 = (float*)val1;
            float_val_2 = (float*)val2;

            return *float_val_1 >= *float_val_2 ? CR_TRUE : CR_FALSE;

        case CT_TIME:
            time_val1 = (unsigned long*)val1;

            if (val2 == NULL)
            {
                unsigned long epoche = (unsigned long)time(NULL);
                time_val2 = &epoche;
            }
            else
            {
                time_val2 = (unsigned long*)val2;
            }

            return *time_val1 >= *time_val2 ? CR_TRUE : CR_FALSE;

        case CT_HASH_FN:
        case CT_ID:
        default:
            return CR_NOT_SUPPORTED;
    }
}

/*
 *  Function: check_lte
 *  Description: Check if first param is less than second. In case when
 *               time needs to be checked, only first parameter can be
 *               sent, function will get the current time (if val2 is NULL).
 *  Parameters: val1 - first value to check
 *              val2 - second value to check
 *              type - type of params
 *  Returns: CR_TRUE - less
 *           CR_FALSE - greater or equale
 *           CR_NOT_SUPPORTED - not supported type
 *           CR_BAD_ARG - bad argument
 */
Validator_check_res_e check_lte(void *val1, void *val2, Validator_cmp_types_e type)
{
    char *tok;
    int *int_val_1;
    int *int_val_2;
    float *float_val_1;
    float *float_val_2;
    unsigned long *time_val1;
    unsigned long *time_val2;

    if (val1 == NULL)
    {
        return CR_BAD_ARG;
    }

    switch (type)
    {
        case CT_INT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            int_val_1 = (int*)val1;
            int_val_2 = (int*)val2;

            return *int_val_1 < *int_val_2 ? CR_TRUE : CR_FALSE;

        case CT_FLOAT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            float_val_1 = (float*)val1;
            float_val_2 = (float*)val2;

            return *float_val_1 < *float_val_2 ? CR_TRUE : CR_FALSE;

        case CT_TIME:
            time_val1 = (unsigned long*)val1;

            if (val2 == NULL)
            {
                unsigned long epoche = (unsigned long)time(NULL);
                time_val2 = &epoche;
            }
            else
            {
                time_val2 = (unsigned long*)val2;
            }

            return *time_val1 < *time_val2 ? CR_TRUE : CR_FALSE;

        case CT_HASH_FN:
        case CT_ID:
        default:
            return CR_NOT_SUPPORTED;
    }
}

/*
 *  Function: check_gte
 *  Description: Check if first param is greater than second. In case when
 *               time needs to be checked, only first parameter can be
 *               sent, function will get the current time (if val2 is NULL).
 *  Parameters: val1 - first value to check
 *              val2 - second value to check
 *              type - type of params
 *  Returns: CR_TRUE - greater
 *           CR_FALSE - less or equale
 *           CR_NOT_SUPPORTED - not supported type
 *           CR_BAD_ARG - bad argument
 */
Validator_check_res_e check_gte(void *val1, void *val2, Validator_cmp_types_e type)
{
    char *tok;
    int *int_val_1;
    int *int_val_2;
    float *float_val_1;
    float *float_val_2;
    unsigned long *time_val1;
    unsigned long *time_val2;

    if (val1 == NULL)
    {
        return CR_BAD_ARG;
    }

    switch (type)
    {
        case CT_INT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            int_val_1 = (int*)val1;
            int_val_2 = (int*)val2;

            return *int_val_1 > *int_val_2 ? CR_TRUE : CR_FALSE;

        case CT_FLOAT:
            if (val2 == NULL)
            {
                return CR_BAD_ARG;
            }

            float_val_1 = (float*)val1;
            float_val_2 = (float*)val2;

            return *float_val_1 > *float_val_2 ? CR_TRUE : CR_FALSE;

        case CT_TIME:
            time_val1 = (unsigned long*)val1;

            if (val2 == NULL)
            {
                unsigned long epoche = (unsigned long)time(NULL);
                time_val2 = &epoche;
            }
            else
            {
                time_val2 = (unsigned long*)val2;
            }

            return *time_val1 > *time_val2 ? CR_TRUE : CR_FALSE;

        case CT_HASH_FN:
        case CT_ID:
        default:
            return CR_NOT_SUPPORTED;
    }
}

/*
 *  Function: check_gocdoc_object
 *  Description: Recursively check GoC and DoC objects
 *  Parameters: _tokens - tokens array
 *              obj_idx - index of the object
 *              max_idx - maximum index
 *              policy_data - policy content
 *  Returns: flags whether att. list and operation are found or not
 */
static int check_gocdoc_object(jsmntok_t* tokens, int obj_idx, int max_idx, const char* policy_data)
{
    Validator_policy_docgoc_level_e ret = DG_HAS_NONE;

    int next_object_index = obj_idx;

    while (next_object_index >= 0)
    {
        for (int j = next_object_index; j < max_idx; j++)
        {
            if (strncmp(&policy_data[tokens[j].start], "attribute_list", strlen("attribute_list")) == 0 &&
                    tokens[j].size == 1 &&
                    tokens[j + 1].type == JSMN_ARRAY)
            {
                // check attribute list array recursively
                if (check_gocdoc_object(tokens, j + 2, Parser_end_of_current_idx(tokens, j, max_idx), policy_data) == (DG_HAS_ATTLIST | DG_HAS_OPP))
                {
                    // return DG_HAS_ATTLIST flag only if child objects are correct
                    ret |= DG_HAS_ATTLIST;
                }
                else
                {
                    // if at least one object is invalid, retrun fault state at once
                    return DG_HAS_NONE;
                }

                // jump to next same-level token
                int next_idx = Parser_next_key_sibling_idx(tokens, j, max_idx);
                j = next_idx - 1;
                if (next_idx < 0)
                {
                    break;
                }
            }
            else if (strncmp(&policy_data[tokens[j].start], "operation", strlen("operation")) == 0 &&
                    tokens[j].size == 1 &&
                    tokens[j + 1].type == JSMN_STRING)
            {
                ret |= DG_HAS_OPP;

                // jump to next same-level token
                int next_idx = Parser_next_key_sibling_idx(tokens, j, max_idx);
                j = next_idx - 1;
                if (next_idx < 0)
                {
                    break;
                }
            }
        }

        /* If neither attribute list or operation is found inside of the object,
        it means that bottom level is reached. Check if type and value are present. */
        if (ret == DG_HAS_NONE)
        {
            /* If this element is object, it means that multiple
            sibling objects exist. */
            if (tokens[next_object_index].type == JSMN_OBJECT)
            {
                int next_bot_level_object_index = next_object_index;

                while (next_bot_level_object_index >= 0)
                {
                    bool found = FALSE;

                    for (int j = next_bot_level_object_index; j <= Parser_end_of_current_idx(tokens, next_bot_level_object_index, max_idx); j++)
                    {
                        if (strncmp(&policy_data[tokens[j].start], "type", strlen("type")) == 0 &&
                            strncmp(&policy_data[tokens[j + 2].start], "value", strlen("value")) == 0)
                        {
                            // if type is "time", check if it's valid
                            if (strncasecmp(&policy_data[tokens[j + 1].start], "time", strlen("time")) == 0)
                            {
                                // check if it is start or end time
                                int att_list_idx = Parser_object_parent_idx(tokens, next_bot_level_object_index) - 1;
                                int operation_idx = Parser_next_key_sibling_idx(tokens, att_list_idx, Parser_end_of_current_idx(tokens, att_list_idx, n) + 2);
                                int op_len = tokens[operation_idx + 1].end - tokens[operation_idx + 1].start;
                                char operation_val[op_len];
                                memcpy(operation_val, &policy_data[tokens[operation_idx + 1].start], op_len);
                                Parser_operations_e op = Parser_get_op(operation_val, op_len);

                                // if operation is LTE or LEQ, time is end-time
                                if (op == OP_LTE || op == OP_LEQ)
                                {
                                    // for time to be valid, end-time mustn't be in the past
                                    int time_s_len = tokens[j + 3].end - tokens[j + 3].start;
                                    char time_s[time_s_len];
                                    memcpy(time_s, &policy_data[tokens[j + 3].start], time_s_len);
                                    unsigned long time_ul = strtoul(time_s, NULL, 10);

                                    if (check_lte(&time_ul, NULL, CT_TIME) == CR_TRUE)
                                    {
                                        // break the loop, without setting the found flag
                                        break;
                                    }
                                }
                            }

                            found = TRUE;
                            break;
                        }
                    }

                    if (found)
                    {
                        // using DG_HAS_ATTLIST and DG_HAS_OPP flags to verify that object is correct
                        ret |= (DG_HAS_ATTLIST | DG_HAS_OPP);
                    }
                    else
                    {
                        // if at least one object is invalid, exit the loop and return DG_HAS_NONE flag
                        ret = DG_HAS_NONE;
                        next_object_index = -1;
                        break;
                    }

                    next_bot_level_object_index = Parser_next_object_sibling_idx(tokens, next_bot_level_object_index, max_idx);
                }
            }
            else
            {
                if (strncmp(&policy_data[tokens[next_object_index].start], "type", strlen("type")) == 0 &&
                    strncmp(&policy_data[tokens[next_object_index + 2].start], "value", strlen("value")) == 0)
                {
                    // using DG_HAS_ATTLIST and DG_HAS_OPP flags to verify that object is correct
                    ret |= (DG_HAS_ATTLIST | DG_HAS_OPP);
                }
            }
        }

        next_object_index = Parser_next_object_sibling_idx(tokens, next_object_index, max_idx);
    }

    return ret;
}

/*
 *  Function: Validator_check
 *  Description: Validate policy
 *  Parameters: policy_data - policy content
 *              report - validation report
 *  Returns: void
 */
void Validator_check(const char* policy_data, Validator_report_t* report)
{
    jsmn_parser parser;
    jsmn_init(&parser);
    memset(report, 0, sizeof(Validator_report_t));

    jsmntok_t tokens[VALIDATOR_MAX_TOKENS];
    n = jsmn_parse(&parser, policy_data, strlen(policy_data), tokens, VALIDATOR_MAX_TOKENS);

    if (n > 0)
    {
        report->valid_json = 1;
    }
    else
    {
        switch(n)
        {
            case JSMN_ERROR_NOMEM:
                printf("JSMN_ERROR_NOMEM\n");
                break;
            case JSMN_ERROR_INVAL:
                printf("JSMN_ERROR_INVAL\n");
                break;
            case JSMN_ERROR_PART:
                printf("JSMN_ERROR_PART\n");
                break;
            default:
                break;
        }
    }

    int proper_format = 1;

    // Top level token is key string "policy"
    if (strncmp(&policy_data[tokens[0].start], "policy", strlen("policy")) != 0 || tokens[0].size != 1)
    {
        proper_format = 0;
    }
    else
    {
        // Value token of top level "policy" token is object
        if (tokens[1].type != JSMN_OBJECT || tokens[1].size != 4) //TODO: Check can size of 4 differ
        {
            proper_format = 0;
        }
        else
        {
            // Second level tokens
            int policy_object_idx = -1;
            int policy_object_end_idx = -1;
            Validator_policy_first_level_e found_everyone = FL_HAS_NONE;
            for (int i = 2; i < n; i++)
            {
                if (tokens[i].size != 1)
                {
                    continue;
                }

                // Check that token is a key, next token is value token and it is of correct type
                if (strncmp(&policy_data[tokens[i].start], "cost", strlen("cost")) == 0 &&
                        tokens[i].size == 1 &&
                        tokens[i + 1].size == 0 &&
                        tokens[i + 1].type == JSMN_STRING)
                {
                    char cost_s[COST_STR_SIZE];
                    memcpy(cost_s, &policy_data[tokens[i + 1].start], COST_STR_SIZE * sizeof(char));
                    float cost_f = strtof(cost_s, NULL);
                    float cost_min = 0.0;

                    if (check_geq(&cost_f, &cost_min, CT_FLOAT) == CR_TRUE)
                    {
                        found_everyone |= FL_HAS_COST;
                    }

                    int next_idx = Parser_next_key_sibling_idx(tokens, i, n);
                    i = next_idx - 1;
                    if (next_idx < 0)
                    {
                        break;
                    }
                }
                else if (strncmp(&policy_data[tokens[i].start], "hash_function", strlen("hash_function")) == 0 &&
                        tokens[i].size == 1 &&
                        tokens[i + 1].size == 0 &&
                        tokens[i + 1].type == JSMN_STRING)
                {
                    char hash_fn_s[tokens[i + 1].end - tokens[i + 1].start];
                    memcpy(hash_fn_s, &policy_data[tokens[i + 1].start], (tokens[i + 1].end - tokens[i + 1].start) * sizeof(char));

                    if (check_eq(&hash_fn_s, NULL, CT_HASH_FN) == CR_TRUE)
                    {
                        found_everyone |= FL_HAS_HASH;
                    }

                    int next_idx = Parser_next_key_sibling_idx(tokens, i, n);
                    i = next_idx - 1;
                    if (next_idx < 0)
                    {
                        break;
                    }
                }
                else if (strncmp(&policy_data[tokens[i].start], "policy_id", strlen("policy_id")) == 0 &&
                        tokens[i].size == 1 &&
                        tokens[i+1].size == 0 &&
                        tokens[i+1].type == JSMN_STRING)
                {
                    // @TODO check if value token is in valid range, type, etc
                    found_everyone |= FL_HAS_POLID;

                    int next_idx = Parser_next_key_sibling_idx(tokens, i, n);
                    i = next_idx - 1;
                    if (next_idx < 0)
                    {
                        break;
                    }
                }
                else if (strncmp(&policy_data[tokens[i].start], "policy_object", strlen("policy_object")) == 0 &&
                        tokens[i].size == 1 &&
                        tokens[i+1].type == JSMN_OBJECT)
                {
                    found_everyone |= FL_HAS_POLOB;
                    policy_object_idx = i;

                    int next_idx = -1;
                    policy_object_end_idx = Parser_end_of_current_idx(tokens, i, n);
                    next_idx = Parser_next_key_sibling_idx(tokens, i, n);
                    i = next_idx - 1;
                    if (next_idx < 0)
                    {
                        break;
                    }
                }
            }

            if (found_everyone != FL_HAS_ALL)
            {
                proper_format = 0;
            }
            else
            {
                // policy_object top level tokens
                if (tokens[policy_object_idx + 1].type != JSMN_OBJECT ||
                        tokens[policy_object_idx + 1].size < POL_OBJC_MIN_MEMBERS)
                {
                    proper_format = 0;
                }
                else
                {
                    Validator_policy_docgoc_level_e check_doc = DG_HAS_NONE;
                    Validator_policy_docgoc_level_e check_goc = DG_HAS_NONE;
                    Validator_policy_docgoc_level_e found_doc_goc = DG_HAS_NONE;

                    /* policy_goc or policy_doc must contain keys "attribute_list" and "operation"
                       attribute_list value may be one of two types of arrays:
                        - array of attributes
                        - array of objects which contain attribute_list and operation */
                    for (int i = policy_object_idx; i < policy_object_end_idx; i++)
                    {
                        if (strncmp(&policy_data[tokens[i].start], "policy_doc", strlen("policy_doc")) == 0 &&
                                tokens[i].size == 1 &&
                                tokens[i + 1].type == JSMN_OBJECT)
                        {
                            found_doc_goc |= DG_HAS_DOC;

                            check_doc |= check_gocdoc_object(tokens, i + 2, Parser_end_of_current_idx(tokens, i + 1, policy_object_end_idx), policy_data);
                        }
                        else if (strncmp(&policy_data[tokens[i].start], "policy_goc", strlen("policy_goc")) == 0 &&
                                tokens[i].size == 1 &&
                                tokens[i + 1].type == JSMN_OBJECT)
                        {
                            found_doc_goc |= DG_HAS_GOC;

                            check_goc |= check_gocdoc_object(tokens, i + 2, policy_object_end_idx, policy_data);
                        }
                    }

                    found_doc_goc |= (check_doc & check_goc);

                    if (found_doc_goc != DG_HAS_ALL)
                    {
                        proper_format = 0;
                    }
                }
            }
        }
    }

    report->proper_format = proper_format;
}
