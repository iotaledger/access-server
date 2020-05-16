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

#include "cfg_mgr_impl.h"
#include "cfg_mgr_cmn.h"

#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define READ_CHUNK_SIZE 16

int CfgMgrImpl_init_cb(void* in_parameter, CfgMgr_t* configuration)
{
    const char* config_file = (const char*)in_parameter;
    FILE* fp = fopen(config_file, "r");
    if (fp == NULL) return CFG_MGR_INIT_ERROR;

    int new_line = 1;
    int bytes_read = -1;
    int total_bytes_read = 0;
    int token_index = 0;
    int comment_line = 0;
    CfgMgr_token_t *this_token = &configuration->tokens[token_index];
    CfgMgr_token_t *group_token = &configuration->tokens[token_index];
    configuration->tokens_count = 0;
    do {
        this_token = &configuration->tokens[token_index];
        char *this_chunk = configuration->data + total_bytes_read;
        bytes_read = fread(this_chunk, 1, READ_CHUNK_SIZE, fp);
        for(int i = 0; i < bytes_read; i++)
        {
            if (new_line == 1 && this_chunk[i] == '#') // comment line
            {
                comment_line = 1;
                new_line = 0;
            }
            else if (new_line == 1)
            {

                if (this_chunk[i] == '[') // start of group token
                {
                    group_token = this_token;
                    this_token->start = total_bytes_read + i + 1;
                    this_token->end = total_bytes_read + i + 1;
                    this_token->size = 0;
                    this_token->level = CFG_MGR_TOKEN_GROUP;
                }
                else // start of option=value token
                {
                    group_token->size++;

                    int new_token_start = total_bytes_read + i;
                    this_token->start = new_token_start;
                    this_token->end = new_token_start;
                    this_token->size = 1;
                    this_token->eq_sign_idx = new_token_start;
                    this_token->level = CFG_MGR_TOKEN_OPTION;
                }
                new_line = 0;
            }
            else if (this_chunk[i] == '=') // start of value
            {
                this_token->eq_sign_idx = total_bytes_read + i;
            }
            else if (this_chunk[i] == '\n') // end of token
            {
                if (comment_line == 1)
                {
                    comment_line = 0;
                }
                else
                {
                    if (this_token == group_token)
                    {
                        this_token->end = total_bytes_read + i - 1;
                    }
                    else
                    {
                        this_token->end = total_bytes_read + i;
                    }
                    configuration->tokens_count++;
                    token_index++;
                    this_token = &configuration->tokens[token_index];
                }
                new_line = 1;
            }
        }
        total_bytes_read += bytes_read;
    } while (bytes_read == READ_CHUNK_SIZE);

    fclose(fp);
    return CFG_MGR_OK;
}

int CfgMgrImpl_get_string_cb(CfgMgr_t* configuration, const char* module_name, const char* option_name, char* option_value, size_t option_value_size)
{
    int found_module = -1;
    int found_config = -1;
    char* cfg_data = configuration->data;
    CfgMgr_token_t *tok;

    for (int i = 0; i < configuration->tokens_count; i++)
    {
        tok = &configuration->tokens[i];

        if (tok->level == CFG_MGR_TOKEN_GROUP && strncmp(module_name, &cfg_data[tok->start], tok->end - tok->start) == 0)
        {
            found_module = i;
            break;
        }
    }

    if (found_module == -1) return CFG_MGR_GROUP_NOT_FOUND;

    for (int i = 0; i < configuration->tokens[found_module].size; i++)
    {
        int idx = found_module+1+i;
        tok = &configuration->tokens[idx];
        if (strncmp(option_name, &cfg_data[tok->start], tok->eq_sign_idx - tok->start) == 0)
        {
            found_config = idx;
            break;
        }
    }

    if (found_config == -1) return CFG_MGR_OPTION_NOT_FOUND;
    size_t copy_len = MIN(tok->end - tok->eq_sign_idx - 1, option_value_size - 1);
    strncpy(option_value, &cfg_data[tok->eq_sign_idx + 1], copy_len);
    option_value[copy_len + 1] = '\0';

    return CFG_MGR_OK;
}

int CfgMgrImpl_get_int_cb(CfgMgr_t* configuration, const char* module_name, const char* option_name, int* option_value)
{
    const size_t string_value_len = 32;
    char string_value[string_value_len];
    int status = CfgMgrImpl_get_string_cb(configuration, module_name, option_name, string_value, string_value_len);
    if (CFG_MGR_OK != status) return status;

    *option_value = atoi(string_value);

    return CFG_MGR_OK;
}

int CfgMgrImpl_get_float_cb(CfgMgr_t* configuration, const char* module_name, const char* option_name, float* option_value)
{
    const size_t string_value_len = 32;
    char string_value[string_value_len];
    int status = CfgMgrImpl_get_string_cb(configuration, module_name, option_name, string_value, string_value_len);
    if (CFG_MGR_OK != status) return status;

    *option_value = atof(string_value);

    return CFG_MGR_OK;
}
