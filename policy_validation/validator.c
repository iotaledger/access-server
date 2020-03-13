/***************************************************************************
 * Project:     Remote Access Delegation
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
 *****************************************************************************/

/***************************************************************************
 * INCLUDES
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "validator.h"

#define JSMN_HEADER
#include "jsmn.h"

/***************************************************************************
 * DEFINES
****************************************************************************/
#define VALIDATOR_TOKEN_STR_SIZE 64

/***************************************************************************
 * GLOBAL VARIABLES
****************************************************************************/
char token_str[VALIDATOR_TOKEN_STR_SIZE];

/***************************************************************************
 * FUNCTION IMPLEMENTATIONS
****************************************************************************/

/*
 *  Function: next_key_sibling_idx
 *  Description: Get index of next key sibling token
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: ID of the next sibling token
 */
static int next_key_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx)
{
    int next_idx = -1;
    // only key tokens have size 1
    if (_tokens[cur_idx].size == 1)
    {
        for (int i = cur_idx + 1; i < max_idx; i++)
        {
            if (_tokens[i].start >= _tokens[cur_idx + 1].end)
            {
                next_idx = i;
                break;
            }
        }
    }
    return next_idx;
}

/*
 *  Function: next_object_sibling_idx
 *  Description: Get index of next object sibling token
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: ID of the next sibling token
 */
static int next_object_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx)
{
    int next_idx = -1;

    if (_tokens[cur_idx].type == JSMN_OBJECT)
    {
        for (int i = cur_idx + 1; i < max_idx; i++)
        {
            if (_tokens[i].start >= _tokens[cur_idx].end)
            {
                next_idx = i;
                break;
            }
        }
    }
    return next_idx;
}

/*
 *  Function: end_of_current_idx
 *  Description: Get last index in current object
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: End position of the token
 */
static int end_of_current_idx(jsmntok_t* _tokens, int cur_idx, int max_idx)
{
    int end_of_current = -1;
    for (int i = cur_idx + 1; i < max_idx; i++)
    {
        if (_tokens[i].end <= _tokens[cur_idx + 1].end)
        {
            end_of_current = i;
        }
        if (_tokens[i].start >= _tokens[cur_idx+1].end)
        {
            break;
        }
    }
    return end_of_current;
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
    
    for (int j = obj_idx; j < max_idx; j++)
    {
        if (strncmp(&policy_data[tokens[j].start], "attribute_list", strlen("attribute_list")) == 0 &&
                tokens[j].size == 1 &&
                tokens[j + 1].type == JSMN_ARRAY)
        {
            // check attribute list array recursively
            if (check_gocdoc_object(tokens, j + 2, end_of_current_idx(tokens, j + 1, max_idx), policy_data) == (DG_HAS_ATTLIST | DG_HAS_OPP))
            {
                // return DG_HAS_ATTLIST flag only if child objects are correct
                ret |= DG_HAS_ATTLIST;
            }

            // jump to next same-level token
            int next_idx = next_key_sibling_idx(tokens, j, max_idx);
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
            int next_idx = next_key_sibling_idx(tokens, j, max_idx);
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
        if (tokens[obj_idx].type == JSMN_OBJECT)
        {
            int next_object_index = obj_idx;

            while (next_object_index >= 0)
            {
                bool found = FALSE;

                for (int j = next_object_index; j <= end_of_current_idx(tokens, next_object_index, max_idx); j++)
                {
                    if (strncmp(&policy_data[tokens[j].start], "type", strlen("type")) == 0 &&
                        strncmp(&policy_data[tokens[j + 2].start], "value", strlen("value")) == 0)
                    {
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
                    break;
                }
                

                next_object_index = next_object_sibling_idx(tokens, next_object_index, max_idx);
            }
        }
        else
        {
            if (strncmp(&policy_data[tokens[obj_idx].start], "type", strlen("type")) == 0 &&
                strncmp(&policy_data[tokens[obj_idx + 2].start], "value", strlen("value")) == 0)
            {
                // using DG_HAS_ATTLIST and DG_HAS_OPP flags to verify that object is correct
                ret |= (DG_HAS_ATTLIST | DG_HAS_OPP);
            }
        }
        
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
    int n = jsmn_parse(&parser, policy_data, strlen(policy_data), tokens, VALIDATOR_MAX_TOKENS);

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
                    // @TODO check if value token is in valid range, type, etc
                    found_everyone |= FL_HAS_COST;

                    int next_idx = next_key_sibling_idx(tokens, i, n);
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
                    // @TODO check if value token is in valid range, type, etc
                    found_everyone |= FL_HAS_HASH;

                    int next_idx = next_key_sibling_idx(tokens, i, n);
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

                    int next_idx = next_key_sibling_idx(tokens, i, n);
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
                    policy_object_end_idx = end_of_current_idx(tokens, i, n);
                    next_idx = next_key_sibling_idx(tokens, i, n);
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
                        tokens[policy_object_idx + 1].size != 2) //FIXME - not sure if it's realy 2
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

                            check_doc |= check_gocdoc_object(tokens, i + 2, end_of_current_idx(tokens, i + 1, policy_object_end_idx), policy_data);
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
