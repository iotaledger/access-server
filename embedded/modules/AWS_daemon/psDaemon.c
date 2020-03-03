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
 * \file psDaemon.c
 * \brief
 * Implementation of the policy store daemon
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 04.09.2018. Initial version.
 ****************************************************************************/

////////////////////////
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "psDaemon.h"

#include "Dlog.h"
#include "policystore.h"
#include "json_parser.h"
#include "tcp_client.h"
#include "resolver.h"

////////////////////////

#define TRUE (1)
#define FALSE (0)

static char policy_store_address[128] = "\0";
static int policy_store_port = 6007;

////////////////////////

static const char PSD_response[] = "response";
static const char PSD_policy_store_id[] = "policyStoreId";
static const char PSD_OpenBracket = '{';
static const char PSD_OpenBracket2 = '[';
static const char PSD_ClosedBracket = '}';
static const char PSD_ClosedBracket2 = ']';
static const char PSD_Comma = ',';
static const char PSD_QuotationMark = '"';
static const char PSD_Colon = ':';
static const char PSD_Space = ' ';

static char PolicyList[AWS_REPLY_LIST_SIZE] = {0, };
static unsigned int PolicyList_len = 0;
static unsigned int newPolicyList = 0;

static char Policy[AWS_REPLY_POLICY_SIZE] = {0, };
static unsigned int Policy_len = 0;
static unsigned int newPolicy = 0;


static int hasPoliciesOnService = 0;

static char action_ps[] = "<Policy service connection>";

///////////////////////////////////
///////////////////////////////////

static void print_data(char *name, unsigned char *data, int len);

static unsigned char re_buffer[64*1024] = {0, };
static int re_length = 0;

static unsigned char wr_buffer[64*1024] = {0, };
static int wr_length = 0;

static void getServiceIpAddress(char *);
static int hostname_to_ip(const char *, char *);


// TODO: move to util module
void print_data2(char *name, unsigned char *data, int len)
{
    int i;
    printf("\n%s (%d) = {", name, len);
    for (i = 0; i < len-1; i++)
    {
        printf("0x%02X, ", data[i]);
    }
    printf("0x%02X};\n\n", data[len-1]);
}

///////////////////////////////////
///////////////////////////////////


static unsigned int psGetPolicies(void);

static int isThisKeyWord(const char *key_word, char *start_p, char *end_p) {
    int ret = FALSE;

    int key_word_length = strlen(key_word);
    int word_length = end_p - start_p;

    if (key_word_length == word_length) {
        ret = TRUE;
        for (int i = 0; i < key_word_length; i++) {
            if (key_word[i] != start_p[i]) {
                ret = FALSE;
                break;
            }
        }
    }

    return ret;
}

static char* findChar(char char_to_find, char *start_p, char *end_p, int skip) {
    char *ret = NULL;

    if (!skip) {
        while (*start_p == PSD_Space)
            if (++start_p == end_p)
                break;

        if (*start_p == char_to_find)
            ret = start_p;
    } else {
        while (start_p != end_p) {
            if (*start_p == char_to_find) {
                ret = start_p;
                break;
            }
            ++start_p;
        }
    }

    return ret;
}

static int searchKeyWord(char **start_p, char **end_p, char *end_all_p) {
    *start_p = findChar(PSD_QuotationMark, *start_p, end_all_p, 0);
    if (NULL == *start_p)
        return 1;

    ++*start_p;
    *end_p = *start_p;

    *end_p = findChar(PSD_QuotationMark, *end_p, end_all_p, 1);
    if (NULL == *end_p)
        return 1;

    return 0;
}

static int parsePSDResponse(char **start_p, char **end_p, char *end_all_p) { // Key response
    int ret = 0;
    int done = 0;

    *start_p = findChar(PSD_Colon, *start_p, end_all_p, 0);
    if (NULL == *start_p)
        return 1; // No colon
    if (++*start_p > end_all_p)
        return 1; // End

    *start_p = findChar(PSD_OpenBracket2, *start_p, end_all_p, 0);
    if (NULL == *start_p)
        return 1; // No openBracket
    if (++*start_p > end_all_p)
        return 1; // End
    do {
        ret = searchKeyWord(start_p, end_p, end_all_p);
        if (ret)
            return 1; // No keyword

        *start_p = *end_p;
        if (++*start_p > end_all_p)
            return 1; // End

        while (**start_p == PSD_Space)
            if (++*start_p > end_all_p)
                return 1; // End

        if (**start_p == PSD_ClosedBracket2)
            done = 1;
        else {
            *start_p = findChar(PSD_Comma, *start_p, end_all_p, 0);
            if (NULL == *start_p)
                return 1; // No Comma
        }
        if (++*start_p > end_all_p)
            return 1; // End
    } while (!done);

    return 0;
}

static int parsePolicyList() {
    char *start_p = PolicyList;
    char *end_p = NULL;
    char *end_all_p = PolicyList + PolicyList_len - 1;
    int ret = 0;
    int foundResponse = 0;
    int foundPID = 0;

    start_p = findChar(PSD_OpenBracket, start_p, end_all_p, 0);
    if (NULL == start_p)
        return 1; // No bracket

    while (!(foundResponse && foundPID)) {

        if (++start_p > end_all_p)
            return 1; // End
        ret = searchKeyWord(&start_p, &end_p, end_all_p);
        if (ret)
            return 1; // No keyword

        ret = isThisKeyWord(PSD_response, start_p, end_p);
        if (TRUE == ret) { // Key response
            if (foundResponse)
                return 1; //Already have this

            start_p = end_p;
            if (++start_p > end_all_p)
                return 1; // End

            ret = parsePSDResponse(&start_p, &end_p, end_all_p);
            if (ret)
                return 1; // No keyword

            foundResponse = 1;

        } else {
            ret = isThisKeyWord(PSD_policy_store_id, start_p, end_p);
            if (TRUE == ret) { // Key policy_store_id
                if (foundPID)
                    return 1; //Already have this

                start_p = end_p;
                if (++start_p > end_all_p)
                    return 1; // End
                start_p = findChar(PSD_Colon, start_p, end_all_p, 0);
                if (NULL == start_p)
                    return 1; // No colon
                if (++start_p > end_all_p)
                    return 1; // End

                ret = searchKeyWord(&start_p, &end_p, end_all_p);
                if (ret)
                    return 1; // No keyword

                start_p = end_p;

                if (++start_p > end_all_p)
                    return 1; // End

                foundPID = 1;
            } else {
                return 1;
            }
        }

        while (*start_p == PSD_Space)
            if (++start_p > end_all_p)
                return 1; // End

        if (*start_p == PSD_ClosedBracket) {
            if (foundResponse && foundPID) {
                return 0;
            }
        } else {
            start_p = findChar(PSD_Comma, start_p, end_all_p, 0);
            if (NULL == start_p)
                return 1; // No Comma
        }
    }

    return 1;
}

static jsmn_parser p;
static jsmntok_t t[256];

static int r = 0;

static void parsePolicy()
{
    char *policy_cost = NULL;
    int policy_cost_len = 0;
    char policy_id_buff[64] = {0, };
    int policy_id_len = 0;
    char *policy_buff = NULL;
    int policy_len = 0;
    int policy_retrieved = 0;

    jsmn_init(&p);
    r = jsmn_parse(&p, Policy, strlen(Policy), t, sizeof(t)/sizeof(t[0]));

    if (r < 0) {
        Dlog_printf("Failed to parse policy JSON: %d\n", r);
        return;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        Dlog_printf("Object expected in policy\n");
        return;
    }

    if(memcmp(Policy + t[1].start, "error", 5) == 0)
    {
        Dlog_printf("\nPolicy not found!");
    }
    else if(memcmp(Policy + t[1].start, "policy", 6) == 0)
    {
        for (int i = 0; i<r; i++)
        {
            if (memcmp(Policy + t[i].start, "policy_id", 9) == 0) {
                policy_id_len = t[i+1].end - t[i+1].start;
                memcpy(policy_id_buff, Policy + t[i+1].start, policy_id_len);
                policy_retrieved++;
                break;
            }
        }
        for (int i = 0; i<r; i++)
        {
            if (memcmp(Policy + t[i].start, "policy_object", 13) == 0) {
                policy_len = t[i+1].end - t[i+1].start;
                policy_buff = calloc(policy_len+1, 1);
                memcpy(policy_buff, Policy + t[i+1].start, policy_len);
                policy_retrieved++;
                break;
            }
        }
        for (int i = 0; i<r; i++)
        {
            if (memcmp(Policy + t[i].start, "cost", 4) == 0) {
                policy_cost_len = t[i+1].end - t[i+1].start;
                policy_cost = calloc(policy_len+1, 1);
                memcpy(policy_cost, Policy + t[i+1].start, policy_cost_len);
                policy_retrieved++;
                break;
            }
        }
        if (policy_retrieved == 3) {
            Dlog_printf("\nPut policy\n");
            PolicyStore_put_policy_from_aws(policy_id_buff, policy_id_len, policy_buff, policy_len, policy_cost, policy_cost_len);
        }
    }

    if (policy_buff != NULL) free(policy_buff);
}

static void getPolicyServiceResponse(char *ascii, int len)
{
    memcpy(Policy, ascii, len);
    memcpy(Policy + len, "\0", 1);

    parsePolicy();

    newPolicy = 1;

    // do while there is policies in the policy store
    //psGetPolicies();
}

static void getPolicy(char *policy_id)
{
    //ip_addr_t destIP;
    static char getPolicyReq[AWS_REQ_GET_LIST_SIZE] = {0, };
    int port = policy_store_port;

    snprintf(getPolicyReq, AWS_REQ_GET_LIST_SIZE, "{\"cmd\": \"get_policy\",\"policyId\":\"%.*s\"}", 64, policy_id);
/*
    IP4_ADDR(&destIP, 52, 59, 214, 233);
    tcp_set_request(getPolicyReq, sizeof(getPolicyReq), getPolicyServiceResponse);
    tcp_client_connect(&destIP, port);
*/
    int response_length;
    char response[2048];

    //AWS_request(getPolicyReq, strlen(getPolicyReq), response, &response_length);
    char policy_service_address[100];
    getServiceIpAddress(policy_service_address);
    tcp_client_send(getPolicyReq, strlen(getPolicyReq), response, &response_length, policy_service_address, policy_store_port);

    getPolicyServiceResponse(response, response_length);
}

static void getPolicyListResponse(char *ascii, int len)
{
    memcpy(PolicyList, ascii, len);
    memcpy(PolicyList + len, "\0", 1);

    newPolicyList = 1;
    PolicyList_len = len;
}

static char policy_store_version[67] = "0x0";
static char device_id[67] = "123";
//static char policy_store_version[] = "0x9852cf1402b7f22b1eda7267fa839f5e901b9b625fd049c8214dce0cc6aeccd4";

void PSDaemon_set_device_id(const char *new_device_id)
{
    strncpy(device_id, new_device_id, sizeof(device_id));
}

static void getPolicyList(void)
{
    //ip_addr_t destIP;

    char getListReq[AWS_REQ_GET_LIST_SIZE];// = "{\"cmd\": \"get_policy_list\",\"policy_store_id\":\"0x46c13f075d25fea1470b2d3904dffb1ef4e40b371b7542f172015495df8d5ef5\"}";
    snprintf(getListReq, AWS_REQ_GET_LIST_SIZE, "{\"cmd\": \"get_policy_list\",\"policyStoreId\":\"%s\",\"deviceId\":\"%s\"}", policy_store_version, device_id);

    int port = policy_store_port;
    /*
    IP4_ADDR(&destIP, 52, 59, 214, 233);
    tcp_set_request(getListReq, sizeof(getListReq), getPolicyListResponse);
    tcp_client_connect(&destIP, port);
    */

    int response_length;
    char response[2048];

    //int res = AWS_request(getListReq, strlen(getListReq), response, &response_length);

    char policy_service_address[100];
    getServiceIpAddress(policy_service_address);
    int res = tcp_client_send(getListReq, strlen(getListReq), response, &response_length, policy_service_address, policy_store_port);

    if (res != 1)
    {
        getPolicyListResponse(response, response_length);
    }
    else
    {

    }

}

static unsigned int psGetPolicyList(void)
{
    unsigned int ret = PSD_ERROR;

    getPolicyList();

    ret = PSD_GET_PL_DONE;

    return ret;
}

static int num_of_policies = 0;
static int initial_num_of_policies = 0;

static void parsePolicyServiceList()
{
    int policy_list = json_parser_init(PolicyList);

    if(policy_list > 0)
    {
        int response = json_get_value(PolicyList, 0, "response");
        if(response != -1)
        {
            int response_type = get_token_type(response);
            if(response_type == 2)// array = 2
            {
                //should resolve policyID list
                hasPoliciesOnService = 1;
                num_of_policies = get_array_size(2);
                initial_num_of_policies = num_of_policies;

                int ps_id = json_get_value(PolicyList, 0, "policyStoreId");
                memcpy(policy_store_version, PolicyList + get_start_of_token(ps_id), 66);
                memcpy(policy_store_version + 66, "\0", 1);
            }
            else if(response_type == 3)// string = 3
            {
                if(memcmp(PolicyList + get_start_of_token(response), "ok", 2) == 0)
                {
                    //  Dlog_printf("\nPolicy store up to date");
                }
                else if(memcmp(PolicyList + get_start_of_token(response), "none", 4) == 0)
                {
                    //Dlog_printf("\nAWS empty");
                    PolicyStore_clean();
                }
                else
                {
                    Dlog_printf("\nERROR: unknown response");
                }
            }
        }
        else
        {
            char buf[80];

            get_time(buf);

            printf("%s %s\tError: Response from policy service not received\n", buf, action_ps);
        }
    }
    else
    {
        Dlog_printf("\nERROR: parsing");
    }
}

static int POLICY_SERVICE_TRY = 0;
static unsigned int psGetPolicyListDone(void) {

    unsigned int ret = PSD_ERROR;

    POLICY_SERVICE_TRY++;
    //TODO: Policy list received
    if(newPolicyList)
    {
        POLICY_SERVICE_TRY = 0;
        newPolicyList = 0;

        parsePolicyServiceList();

        ret = PSD_GET_PSS;
    }
    else
    {
        if(POLICY_SERVICE_TRY < 3)
        {
            ret = PSD_GET_PL_DONE;
        }
        else
        {
            POLICY_SERVICE_TRY = 0;
            ret = PSD_GET_PL;
        }
    }

    return ret;
}


static unsigned int psGetPolicies(void) {


    unsigned int ret = PSD_ERROR;

    while(num_of_policies > 0)
    {
        int current_policy = num_of_policies - 1;

        //Dlog_printf("\n(%d) ", current_policy);
        getPolicy(PolicyList + get_start_of_token(3 + current_policy));//pol_id_token + 2

        num_of_policies -= 1;

        ret = PSD_GET_PSS;
    }
    return ret;
}

static unsigned int psGetPoliciesDone(void) {
    unsigned int ret = PSD_ERROR;

    if(num_of_policies != 0) {
        ret = PSD_GET_PSS;
    }
    else
    {
        //synchronize_ps_and_AWS(PolicyList, initial_num_of_policies);
        initial_num_of_policies = 0;
        ret = PSD_GET_PL;
    }

    return ret;
}

static unsigned int psInitDaemon(void) {
//    printf("\n0. psInitDaemon");

    unsigned int ret = PSD_ERROR;

    //TODO: Initialize all
    //Dlog_printf("PSD_INIT\r\n");

    ret = PSD_GET_PL;

    return ret;
}

unsigned int psDaemonError(void) {
    unsigned int ret = PSD_ERROR;

    //TODO: Do something if error
    //Dlog_printf("PSD_ERROR\r\n");

    return ret;
}

static unsigned int ps_state = PSD_INIT;

int PSDaemon_do_work()
{
    unsigned int next_state = PSD_ERROR;

    switch(ps_state)
    {
        case PSD_GET_PL:
        {
            next_state = psGetPolicyList();
            break;
        }
        case PSD_GET_PL_DONE:
        {
            psGetPolicyListDone();
            psGetPolicies();
            next_state = PSD_GET_PL;
            initial_num_of_policies = 0;
            break;
        }
        case PSD_INIT: {
            next_state = psInitDaemon();
            break;
        }
        default:
            break;
    }
    ps_state = next_state;

    return 0;
}

void PSDaemon_set_policy_store_address(const char* address) {
    strncpy(policy_store_address, address, 128);
}

void PSDaemon_set_policy_store_port(int port) {
    policy_store_port = port;
}

static void getServiceIpAddress(char *ip_address) {
    hostname_to_ip(policy_store_address, ip_address);
}

static int hostname_to_ip(const char *hostname, char *ip_address)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    he = gethostbyname(hostname);
    if (he == NULL)
    {
        return 1;
    }
    addr_list = (struct in_addr **) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++)
    {
        strcpy(ip_address, inet_ntoa(*addr_list[i]));
        return 0;
    }
}
