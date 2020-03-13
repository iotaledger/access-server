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
//#include "IfxPort_PinMap.h"
#include "resolver.h"
#include "json_parser.h"
//#include "compstatus.h"
#include "Dlog.h"
#include <string.h>

//#define OPEN_TRUNCK &IfxPort_P20_13
//#define LOCK_DOOR &IfxPort_P20_14

#define Dlog_printf printf

void broadcast_status(char *a, char* c, bool is_successfull)
{
    if (a == NULL || c == NULL)
    {
        return;
    }
    
    is_successfull ? Dlog_printf("\n\nAction %s %s performed successfully\n\n", a, c) : Dlog_printf("\n\nAction %s %s failed\n\n", a, c);
}

int ledControl(int decision, char *obligation, char *action, unsigned long start_time, unsigned long end_time)
{
    bool should_log = FALSE;
    
    //TODO: only "log_event" obligation is supported currently
    if(0 == memcmp(obligation, "log_event", strlen("log_event")))
    {
        should_log = TRUE;
    }
    
    if(decision == 1)
    {
        if((0 == memcmp(action,"open_trunk", strlen("open_trunk"))))
        {
            Resolver_action03();
            if(should_log)
            {
                broadcast_status("trunk", "unlock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"open_door", strlen("open_door"))))
        {
            Resolver_action01();
            if(should_log)
            {
                broadcast_status("door", "unlock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"close_door", strlen("close_door"))))
        {
            Resolver_action02();
            if(should_log)
            {
                broadcast_status("door", "lock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"start_engine", strlen("start_engine"))))
        {
            Resolver_action04();
        }
        else if ((0 == memcmp(action, "honk", strlen("honk"))))
        {
            Resolver_action03();
        }
        else if ((0 == memcmp(action, "alarm_on", strlen("alarm_on"))))
        {
            Resolver_action04();
        }
        else if ((0 == memcmp(action, "alarm_off", strlen("alarm_off"))))
        {
            Resolver_action05();
        }
        else if ((0 == memcmp(action, "start_ds_", strlen("start_ds_") - 1))) // Do not count '\0' character for memcmp
        {
            Resolver_action06(action, end_time);
        }
        else if ((0 == memcmp(action, "stop_ds", strlen("stop_ds"))))
        {
            Resolver_action07();
        }
    }
    else
    {

    }

    return 0;
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
        return -2;
    }

    if (json_get_value(request, 0, "cmd") == -1)
    {
        return -3;
    }

    int cmd = json_get_value(request, 0, "cmd");

    if(memcmp(request + get_start_of_token(cmd) , "resolve", strlen("resolve")) == 0)
    {
        if(json_get_value(request, 0, "policy_id") == -1)
        {
            //error invalid request
            return -4;
        }
        else
        {
            return 0;
        }
    }
    else if(memcmp(request + get_start_of_token(cmd) , "get_policy_list", strlen("get_policy_list")) == 0)
    {
        if(json_get_value(request, 0, "user_id") == -1)
        {
            //error invalid request
            return -6;
        }
        else
        {
            return 1;
        }
    }
    else if(memcmp(request + get_start_of_token(cmd) , "enable_policy", strlen("enable_policy")) == 0)
    {
        if(json_get_value(request, 0, "policy_id") == -1)
        {
            //error invalid request
            return -4;
        }
        else
        {
            return 2;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "set_dataset", strlen("set_dataset")) == 0)
    {
        if (json_get_value(request, 0, "dataset_list") == -1)
        {
            return -7;
        }
        else
        {
            return 3;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_dataset", strlen("get_dataset")) == 0)
    {
        return 4;
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_user", strlen("get_user")) == 0)
    {
        if (json_get_value(request, 0, "username") == -1)
        {
            return -8;
        }
        else
        {
            return 5;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_auth_user_id", strlen("get_auth_user_id")) == 0)
    {
        if (json_get_value(request, 0, "username") == -1)
        {
            return -9;
        }
        else
        {
            return 6;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "register_user", strlen("register_user")) == 0)
    {
        if (json_get_value(request, 0, "user") == -1)
        {
            return -10;
        }
        else
        {
            return 7;
        }
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_all_users", strlen("get_all_users")) == 0)
    {
        return 8;
    }
    else if (memcmp(request + get_start_of_token(cmd), "clear_all_users", strlen("clear_all_users")) == 0)
    {
        return 9;
    }
    else
    {
        return -5;
    }
}
