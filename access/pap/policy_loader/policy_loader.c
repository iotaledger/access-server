/*
 * This file is part of the IOTA Access Distribution
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

#include "policy_loader.h"

#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "Dlog.h"
#include "pap.h"
#include "json_parser.h"
#include "time_manager.h"
#include "config_manager.h"
#include "utils.h"

#include "policy_updater.h"

#define POLICY_LOADER_REPLY_LIST_SIZE   (1024)

#define POLICY_LOADER_REPLY_POLICY_SIZE (2048)

/* POLICY_LOADER_STAGES */
#define POLICY_LOADER_ERROR            (0)
#define POLICY_LOADER_INIT             (1)
#define POLICY_LOADER_GET_PL           (2)
#define POLICY_LOADER_GET_PL_DONE      (3)
#define POLICY_LOADER_GET_PSS          (4)
#define POLICY_LOADER_GET_PSS_DONE     (5)

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define POLICY_LOADER_TOK_NUM 256
#define POLICY_LOADER_POL_ID_BUF_LEN 64
#define POLICY_LOADER_STR_LEN 67
#define POLICY_LOADER_POL_FULLY_RETRIEVED 4
#define POLICY_LOADER_ARRAY_TOK_IDX 2
#define POLICY_LOADER_TIME_BUF_LEN 80
#define POLICY_LOADER_MAX_GET_TRY 3


#define POLICY_LOADER_POL_RESPONSE_TYPE_ARRAY 2
#define POLICY_LOADER_POL_RESPONSE_TYPE_STRING 3

static const char POLICY_LOADER_response[] = "response";
static const char POLICY_LOADER_policy_store_id[] = "policyStoreId";
static const char POLICY_LOADER_OpenBracket = '{';
static const char POLICY_LOADER_OpenBracket2 = '[';
static const char POLICY_LOADER_ClosedBracket = '}';
static const char POLICY_LOADER_ClosedBracket2 = ']';
static const char POLICY_LOADER_Comma = ',';
static const char POLICY_LOADER_QuotationMark = '"';
static const char POLICY_LOADER_Colon = ':';
static const char POLICY_LOADER_Space = ' ';

static char g_policy_list[POLICY_LOADER_REPLY_LIST_SIZE] = {0, };
static unsigned int g_policy_list_len = 0;
static unsigned int g_new_policy_list = 0;

static char g_policy[POLICY_LOADER_REPLY_POLICY_SIZE] = {0, };

static char g_action_ps[] = "<policy service connection>";

static int g_task_sleep_time = -1;

static int g_end;

static pthread_t g_thread;

static jsmn_parser p;
static jsmntok_t t[POLICY_LOADER_TOK_NUM];
static int r = 0;

static int cycle_fsm();
static unsigned int receive_policies(void);


static char* find_char(char char_to_find, char *start_p, char *end_p, int skip)
{
    char *ret = NULL;

    if (!skip)
    {
        while (*start_p == POLICY_LOADER_Space)
        {
            if (++start_p == end_p)
            {
                break;
            }
        }

        if (*start_p == char_to_find)
        {
            ret = start_p;
        }
    }
    else
    {
        while (start_p != end_p)
        {
            if (*start_p == char_to_find)
            {
                ret = start_p;
                break;
            }
            ++start_p;
        }
    }

    return ret;
}

static int search_key_word(char **start_p, char **end_p, char *end_all_p)
{
    *start_p = find_char(POLICY_LOADER_QuotationMark, *start_p, end_all_p, 0);
    if (NULL == *start_p)
    {
        return 1;
    }

    ++*start_p;
    *end_p = *start_p;

    *end_p = find_char(POLICY_LOADER_QuotationMark, *end_p, end_all_p, 1);
    if (NULL == *end_p)
    {
        return 1;
    }

    return 0;
}

static int parse_policy_updater_response(char **start_p, char **end_p, char *end_all_p) // Key response
{
    int ret = 0;
    int done = 0;

    *start_p = find_char(POLICY_LOADER_Colon, *start_p, end_all_p, 0);
    if (NULL == *start_p)
    {
        return 1; // No colon
    }

    if (++*start_p > end_all_p)
    {
        return 1; // End
    }

    *start_p = find_char(POLICY_LOADER_OpenBracket2, *start_p, end_all_p, 0);
    if (NULL == *start_p)
    {
        return 1; // No openBracket
    }

    if (++*start_p > end_all_p)
    {
        return 1; // End
    }

    do {
        ret = search_key_word(start_p, end_p, end_all_p);
        if (ret)
        {
            return 1; // No keyword
        }

        *start_p = *end_p;
        if (++*start_p > end_all_p)
        {
            return 1; // End
        }

        while (**start_p == POLICY_LOADER_Space)
        {
            if (++*start_p > end_all_p)
            {
                return 1; // End
            }
        }

        if (**start_p == POLICY_LOADER_ClosedBracket2)
        {
            done = 1;
        }
        else
        {
            *start_p = find_char(POLICY_LOADER_Comma, *start_p, end_all_p, 0);
            if (NULL == *start_p)
            {
                return 1; // No Comma
            }
        }

        if (++*start_p > end_all_p)
        {
            return 1; // End
        }

    } while (!done);

    return 0;
}


static int parse_policy(const char *p_policy, char *policy_buff, size_t *o_policy_len)
{
    char *policy_cost = NULL;
    int policy_cost_len = 0;
    char policy_id_buff[POLICY_LOADER_POL_ID_BUF_LEN] = {0, };
    int policy_id_len = 0;
    int policy_len = 0;
    char *policy_id_signature_buff = NULL;
    int policy_id_signature_len = 0;
    int policy_retrieved = 0;

    jsmn_init(&p);
    r = jsmn_parse(&p, p_policy, strlen(p_policy), t, POLICY_LOADER_TOK_NUM);

    if (r < 0)
    {
        Dlog_printf("Failed to parse policy JSON: %d\n", r);
        return 0;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT)
    {
        Dlog_printf("Object expected in policy\n");
        return 0;
    }

    if(memcmp(p_policy + t[1].start, "error", strlen("error")) == 0)
    {
        Dlog_printf("\nPolicy not found!");
    }
    else if(memcmp(p_policy + t[1].start, "policy", strlen("policy")) == 0)
    {
        for (int i = 0; i<r; i++)
        {
            if (memcmp(p_policy + t[i].start, "policy_id", strlen("policy_id")) == 0)
            {
                policy_id_len = t[i+1].end - t[i+1].start;
                memcpy(policy_id_buff, p_policy + t[i+1].start, policy_id_len);
                policy_retrieved++;
                break;
            }
        }
        for (int i = 0; i<r; i++)
        {
            if (memcmp(p_policy + t[i].start, "policy_object", strlen("policy_object")) == 0)
            {
                policy_len = t[i+1].end - t[i+1].start;
                policy_buff = calloc(policy_len+1, 1);
                memcpy(policy_buff, p_policy + t[i+1].start, policy_len);
                policy_retrieved++;
                break;
            }
        }
        for (int i = 0; i<r; i++)
        {
            if (memcmp(p_policy + t[i].start, "policy_id_signature", strlen("policy_id_signature")) == 0)
            {
                policy_id_signature_len = t[i+1].end - t[i+1].start;
                policy_id_signature_buff = calloc(policy_id_signature_len+1, 1);
                memcpy(policy_id_signature_buff, p_policy + t[i+1].start, policy_id_signature_len);
                policy_retrieved++;
                break;
            }
        }
        for (int i = 0; i<r; i++)
        {
            if (memcmp(p_policy + t[i].start, "cost", strlen("cost")) == 0)
            {
                policy_cost_len = t[i+1].end - t[i+1].start;
                policy_cost = calloc(policy_len+1, 1);
                memcpy(policy_cost, p_policy + t[i+1].start, policy_cost_len);
                policy_retrieved++;
                break;
            }
        }
    }

    if (policy_retrieved == POLICY_LOADER_POL_FULLY_RETRIEVED)
    {
        Dlog_printf("\nPut policy\n");
    }
    return policy_retrieved == POLICY_LOADER_POL_FULLY_RETRIEVED;
}

static char g_policy_store_version[POLICY_LOADER_STR_LEN] = "0x0";
static char g_device_id[POLICY_LOADER_STR_LEN] = "123";

static int num_of_policies = 0;

static void parse_policy_service_list()
{
    int policy_list = json_parser_init(g_policy_list);

    if(policy_list > 0)
    {
        int response = json_get_value(g_policy_list, 0, "response");
        if(response != -1)
        {
            int response_type = get_token_type(response);
            if(response_type == POLICY_LOADER_POL_RESPONSE_TYPE_ARRAY)
            {
                //should resolve policyID list
                num_of_policies = get_array_size(POLICY_LOADER_ARRAY_TOK_IDX);

                int ps_id = json_get_value(g_policy_list, 0, "policyStoreId");
                memcpy(g_policy_store_version, g_policy_list + get_start_of_token(ps_id), POLICY_LOADER_STR_LEN - 1);
                memcpy(g_policy_store_version + (POLICY_LOADER_STR_LEN - 1), "\0", 1);
            }
            else if(response_type == POLICY_LOADER_POL_RESPONSE_TYPE_STRING)
            {
                if(memcmp(g_policy_list + get_start_of_token(response), "ok", strlen("ok")) == 0)
                {
                    Dlog_printf("\nPolicy store up to date");
                }
                else
                {
                    Dlog_printf("\nERROR: unknown response");
                }
            }
        }
        else
        {
            char buf[POLICY_LOADER_TIME_BUF_LEN];

            getStringTime(buf, POLICY_LOADER_TIME_BUF_LEN);
            printf("%s %s\tError: Response from policy service not received\n", buf, g_action_ps);
        }
    }
    else
    {
        Dlog_printf("\nERROR: parsing");
    }
}

static int g_fsm_try_counter = 0;

static unsigned int fsm_get_policy_list_done(void)
{

    unsigned int ret = POLICY_LOADER_ERROR;

    g_fsm_try_counter++;
    //TODO: policy list received
    if(g_new_policy_list)
    {
        g_fsm_try_counter = 0;
        g_new_policy_list = 0;

        parse_policy_service_list();

        ret = POLICY_LOADER_GET_PSS;
    }
    else
    {
        if(g_fsm_try_counter < POLICY_LOADER_MAX_GET_TRY)
        {
            ret = POLICY_LOADER_GET_PL_DONE;
        }
        else
        {
            g_fsm_try_counter = 0;
            ret = POLICY_LOADER_GET_PL;
        }
    }

    return ret;
}


static unsigned int receive_policies(void)
{
    unsigned int ret = POLICY_LOADER_ERROR;
    char *policy_buff = NULL;
    int policy_len = 0;
    int status = 0;
    while(num_of_policies > 0)
    {
        int current_policy = num_of_policies - 1;

        PolicyUpdater_get_policy(g_policy_list + get_start_of_token(3 + current_policy), g_policy);
        int status = parse_policy(g_policy, policy_buff, &policy_len);
        if (status == 1)
        {
            PAP_add_policy(policy_buff, policy_len, NULL);
        }
        if (policy_buff != NULL)
        {
            free(policy_buff);
        }

        num_of_policies -= 1;

        ret = POLICY_LOADER_GET_PSS;
    }
    return ret;
}

static unsigned int fsm_init(void)
{

    unsigned int ret = POLICY_LOADER_ERROR;

    //TODO: Initialize all

    ret = POLICY_LOADER_GET_PL;

    return ret;
}

static unsigned int g_policy_updater_fsm_state = POLICY_LOADER_INIT;

static int cycle_fsm()
{
    unsigned int next_state = POLICY_LOADER_ERROR;

    switch(g_policy_updater_fsm_state)
    {
        case POLICY_LOADER_GET_PL:
            PolicyUpdater_get_policy_list(g_policy_store_version, g_device_id, g_policy_list, &g_policy_list_len, &g_new_policy_list);
            next_state = POLICY_LOADER_GET_PL_DONE;
            break;
        case POLICY_LOADER_GET_PL_DONE:
            fsm_get_policy_list_done();
            receive_policies();
            next_state = POLICY_LOADER_GET_PL;
            break;
        case POLICY_LOADER_INIT:
            next_state = fsm_init();
            break;
        default:
            break;
    }
    g_policy_updater_fsm_state = next_state;

    return 0;
}

void PolicyLoader_init()
{
    ConfigManager_get_option_string("config", "device_id", g_device_id, POLICY_LOADER_STR_LEN);
    int status = ConfigManager_get_option_int("config", "thread_sleep_period", &g_task_sleep_time);
    if (status != CONFIG_MANAGER_OK) g_task_sleep_time = 1000; // 1 second
}


static void *policy_loader_thread_function(void *arg);

int PolicyLoader_start()
{
    g_end = 0;
    pthread_create(&g_thread, NULL, policy_loader_thread_function, NULL);
    return 0;
}

int PolicyLoader_stop()
{
    g_end = 1;
    pthread_join(g_thread, NULL);
    return 0;
}


static void *policy_loader_thread_function(void *arg)
{
    int period_counter = 0;
    while(!g_end)
    {
        if (period_counter++ == (5000000 / g_task_sleep_time) || (5000000 / g_task_sleep_time) == 0) // 5 seconds
        {
            period_counter = 0;
            cycle_fsm();
        }
        usleep(g_task_sleep_time);
    }
}