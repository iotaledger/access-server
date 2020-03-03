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
 * \file bc_daemon.c
 * \brief
 * Blockchain daemon implementation file
 *
 * @Author Robert Cernjanski
 *
 * \notes
 *
 * \history
 * 06.05.2019. Initial version.
 ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <pthread.h>

#include "jsmn.h"
#include "bc_daemon.h"
#include "globals_declarations.h"

static char token_address[128] = "0x4025905055cdc2AddA98D6F198Ec0E06A8E08060";
static int token_send_interval = 10000000; // microseconds
static float token_amount = 0.001;

static char bc_hostname[128] = "http://34.77.82.182";
static int bc_hostname_port = 8888;

static int end_thread = 0;
static pthread_t bc_daemon_thread;
static CURL* curl = 0;
static VehicleDataset_state_t *vd_state;
static char fund_tokens_response[1024];

static jsmn_parser parser;
static jsmntok_t tokens[10];

static void *bc_daemon_thread_function(void *ptr);
static void fund_tokens();
static CURLcode rest_curl_helper(char* command_string, size_t (*callback)(void*, size_t, size_t, void*));
static size_t parse_fund_tokens_response(void *buffer, size_t size, size_t nmemb, void *stream);

void BlockchainDaemon_init(VehicleDataset_state_t *dataset)
{
    vd_state = dataset;

    // init curl
    CURLcode curl_status;
    curl_status = curl_global_init(CURL_GLOBAL_ALL);

    if (curl_status != 0)
    {
        fprintf(stderr, "curl_global_init failed!");
        exit(-1);
    }

    curl = curl_easy_init();
    if (curl == NULL)
    {
        fprintf(stderr, "curl_easy_init failed!");
        exit(-1);
    }
}

void BlockchainDaemon_start()
{
    end_thread = 0;
    if (pthread_create(&bc_daemon_thread, NULL, bc_daemon_thread_function, NULL)){
       fprintf(stderr, "Error creating car gateway thread\n");
    }
}

void BlockchainDaemon_stop()
{
    end_thread = 1;
    pthread_join(bc_daemon_thread, NULL);

    // deinit curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

static void *bc_daemon_thread_function(void *ptr)
{
    int should_fund = 0;
    int period_counter = 0;

    while(end_thread == 0)
    {
        if (period_counter++ == (token_send_interval * 1000)) {
            period_counter = 0;
            should_fund = VehicleDataset_checked_count(vd_state);
            if (should_fund > 0)
                fund_tokens();
        }

        usleep(g_task_sleep_time);
    }
}

static void fund_tokens()
{
    char post_body[1024];
    double amount = 0;

    amount = (double) VehicleDataset_checked_count(vd_state);
    if (amount == 0)
        return;
    amount = amount * token_amount;
    snprintf(post_body, 1024, "{\"address\":\"%s\",\"amount\":%5.3f}", token_address, amount);
    if (curl)
    {
        int res = 0;
        char url[1024];
        struct curl_slist *headers = NULL;

        snprintf(url, 1024, "%s:%d/%s", bc_hostname, bc_hostname_port, "fund");
        curl_easy_setopt(curl, CURLOPT_URL, url);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_fund_tokens_response);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "Failed to fund: %s, reason: %s\n", token_address, curl_easy_strerror(res));
    }
}

static int min(int x, int y)
{
    return x < y ? x : y;
}

static size_t parse_fund_tokens_response(void *buffer, size_t size, size_t nmemb, void *stream)
{
    int num_of_tokens = 0;
    char* response = (char*)buffer;
    size_t bufferlen = strlen(buffer);
    int is_success = 0;
    char tmp_str[50] = "";
    double amount = 0;

    jsmn_init(&parser);
    num_of_tokens = jsmn_parse(&parser, response, bufferlen, tokens, 10);

    for (int i = 0, tok_len = 0; i < num_of_tokens; i++)
    {
        tok_len = tokens[i].end - tokens[i].start;
        int tok_len_2 = -1;
        if (strncmp("error", response + tokens[i].start, min(tok_len, 5)) == 0)
        {
            tok_len_2 = tokens[i+1].end - tokens[i+1].start;
            if (strncmp("false", response + tokens[i+1].start, min(tok_len_2, 5)) == 0) {
                is_success = 1;
                continue;
            }
        }

        if (strncmp("message", response + tokens[i].start, min(tok_len, 7)) == 0)
        {
            strncpy(tmp_str, response + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
            tmp_str[tokens[i + 1].end - tokens[i + 1].start] = '\0';
            continue;
        }
    }


    printf("fund %s: %s\n", token_address, tmp_str);

    return bufferlen;
}

void BlockchainDaemon_set_token_address(const char* new_token_address)
{
    strncpy(token_address, new_token_address, 128);
}

void BlockchainDaemon_set_token_send_interval(int send_interval_us)
{
    token_send_interval = send_interval_us;
}

void BlockchainDaemon_set_token_amount(float new_amount)
{
    token_amount = new_amount;
}

void BlockchainDaemon_set_bc_hostname(const char* new_bc_hostname)
{
    strncpy(bc_hostname, new_bc_hostname, 128);
}

void BlockchainDaemon_set_bc_hostname_port(int port)
{
    bc_hostname_port = port;
}
