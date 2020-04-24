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
 * \project Decentralized Access Control
 * \file authDacHelper.c
 * \brief
 * Implementation of helper functions for pep module
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 02.10.2018. Initial version.
 * 21.02.2020. Added obligations handling
 * 28.02.2020. Added data sharing through action functionality
 ****************************************************************************/

#include "authDacHelper.h"
#include "resolver.h"
#include "json_parser.h"
#include <string.h>

#define Dlog_printf printf

int ledControl(int decision, char *obligation, char *action, unsigned long start_time, unsigned long end_time)
{
    bool should_log = FALSE;
    
    //TODO: only "log_event" obligation is supported currently
    if(0 == memcmp(obligation, "log_event", strlen("log_event")))
    {
        should_log = TRUE;
    }
    
    if (decision == 1)
    {
        // TODO: better handling of end_time parameter
        Resolver_action(action, should_log, &end_time);
    }

    return ADH_NO_ERROR;
}

int sendDecision(int decision, dacSession_t *session)
{
    char grant[] = {"{\"response\":\"access granted\"}"};
    char deny[] = {"{\"response\":\"access denied\"}"};
    char *msg;

    if(decision == 1)
    {
        msg = grant;
    }
    else
    {
        msg = deny;
    }

    return dacSend(session, msg, strlen(msg));
}

int sendDecision_new(int decision, dacSession_t *session, char* response, int size)
{
    return dacSend(session, response, size);
}

int checkMsgFormat_new(const char *request)
{
    if (request == NULL)
    {
        return ADH_ERROR_JSON_NULL;
    }

    if (json_get_value(request, 0, "cmd") == -1)
    {
        return ADH_ERROR_CMD_NOT_FND;
    }

    int cmd = json_get_value(request, 0, "cmd");

    if(memcmp(request + get_start_of_token(cmd) , "resolve", strlen("resolve")) == 0)
    {
        if(json_get_value(request, 0, "policy_id") == -1)
        {
            //error invalid request
            return ADH_ERROR_POLID_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR;
        }
    }
    else if(memcmp(request + get_start_of_token(cmd) , "get_policy_list", strlen("get_policy_list")) == 0)
    {
        if(json_get_value(request, 0, "user_id") == -1)
        {
            //error invalid request
            return ADH_ERROR_USRID_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_GET_POL_LIST;
        }
    }
    else if(memcmp(request + get_start_of_token(cmd) , "enable_policy", strlen("enable_policy")) == 0)
    {
        if(json_get_value(request, 0, "policy_id") == -1)
        {
            //error invalid request
            return ADH_ERROR_POLID_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_EN_POL;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "set_dataset", strlen("set_dataset")) == 0)
    {
        if (json_get_value(request, 0, "dataset_list") == -1)
        {
            return ADH_ERROR_DATASET_LIST_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_SET_DATASET;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_dataset", strlen("get_dataset")) == 0)
    {
        return ADH_NO_ERROR_GET_DATASET;
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_user", strlen("get_user")) == 0)
    {
        if (json_get_value(request, 0, "username") == -1)
        {
            return ADH_ERROR_GETUSR_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_GETUSR;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_auth_user_id", strlen("get_auth_user_id")) == 0)
    {
        if (json_get_value(request, 0, "username") == -1)
        {
            return ADH_ERROR_GETUSRID_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_GETUSRID;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "register_user", strlen("register_user")) == 0)
    {
        if (json_get_value(request, 0, "user") == -1)
        {
            return ADH_ERROR_REGUSR_NOT_FND;
        }
        else
        {
            return ADH_NO_ERROR_REGUSR;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_all_users", strlen("get_all_users")) == 0)
    {
        return ADH_NO_ERROR_GETALLUSR;
    }
    else if (memcmp(request + get_start_of_token(cmd), "clear_all_users", strlen("clear_all_users")) == 0)
    {
        return ADH_NO_ERROR_CLRALLUSR;
    }
    else
    {
        return ADH_ERROR_NEQ_RESOLVE;
    }
}
