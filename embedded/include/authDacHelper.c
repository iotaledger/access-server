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

int ledControl(int decision, char *obligation, char *action)
{
    bool should_log = FALSE;
    
    //TODO: only "log_event" obligation is supported currently
    if(0 == memcmp(obligation, "log_event", 9))
    {
        should_log = TRUE;
    }
    
    if(decision == 1)
    {
        if((0 == memcmp(action,"open_trunk", 10)))
        {
            Resolver_action03();
            if(should_log)
            {
                broadcast_status("trunk", "unlock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"open_door", 9)))
        {
            Resolver_action01();
            if(should_log)
            {
                broadcast_status("door", "unlock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"close_door", 10)))
        {
            Resolver_action02();
            if(should_log)
            {
                broadcast_status("door", "lock", TRUE);
            }
        }
        else if ((0 == memcmp(action,"start_engine", 12)))
        {
            Resolver_action04();
        }
        else if ((0 == memcmp(action, "honk", 4)))
        {
            Resolver_action03();
        }
        else if ((0 == memcmp(action, "alarm_on", 8)))
        {
            Resolver_action04();
        }
        else if ((0 == memcmp(action, "alarm_off", 9)))
        {
            Resolver_action05();
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

    if(memcmp(request + get_start_of_token(cmd) , "resolve", 7) == 0)
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
    else if(memcmp(request + get_start_of_token(cmd) , "get_policy_list", 15) == 0)
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
    else if(memcmp(request + get_start_of_token(cmd) , "enable_policy", 13) == 0)
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
    else if (memcmp(request + get_start_of_token(cmd), "set_dataset", 11) == 0)
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
    else if (memcmp(request + get_start_of_token(cmd), "get_dataset", 11) == 0)
    {
        return 4;
    }
    else if (memcmp(request + get_start_of_token(cmd), "get_user", 8) == 0)
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
    else if (memcmp(request + get_start_of_token(cmd), "get_auth_user_id", 16) == 0)
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
    else if (memcmp(request + get_start_of_token(cmd), "register_user", 13) == 0)
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
    else if (memcmp(request + get_start_of_token(cmd), "get_all_users", 13) == 0)
    {
        return 8;
    }
    else if (memcmp(request + get_start_of_token(cmd), "clear_all_users", 15) == 0)
    {
        return 9;
    }
    else
    {
        return -5;
    }
}
