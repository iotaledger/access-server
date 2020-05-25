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
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <pthread.h>

#include "jsmn.h"
#include "bc_daemon.h"
#include "globals_declarations.h"

#define BC_MAX_STR_LEN 128
#define BC_TOKEN_SEND_INTERVAL_10s 10000000 // in microseconds
#define BC_TOK_ARRAY_SIZE 10
#define BC_URL_LEN 1024
#define BC_INFO_STRING_LEN 64

static char token_address[BC_MAX_STR_LEN] = "";
static int token_send_interval = BC_TOKEN_SEND_INTERVAL_10s;
static float token_amount = -1.;

static char bc_hostname[BC_MAX_STR_LEN] = "";
static int bc_hostname_port = -1;

static int end_thread = 0;
static pthread_t bc_daemon_thread;
static CURL* curl = 0;
static Dataset_state_t *vd_state;

static jsmn_parser parser;
static jsmntok_t tokens[BC_TOK_ARRAY_SIZE];

static void *bc_daemon_thread_function(void *ptr);
static void fund_tokens();
static size_t parse_fund_tokens_response(void *buffer, size_t size, size_t nmemb, void *stream);

void BlockchainDaemon_init(Dataset_state_t *dataset)
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
    if (pthread_create(&bc_daemon_thread, NULL, bc_daemon_thread_function, NULL))
    {
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
        if (period_counter++ == (token_send_interval * 1000))
        {
            period_counter = 0;
            should_fund = Dataset_checked_count(vd_state);
            if (should_fund > 0)
            {
                fund_tokens();
            }
        }

        usleep(g_task_sleep_time);
    }
}

static void fund_tokens()
{
    char post_body[BC_URL_LEN];
    double amount = 0;

    amount = (double) Dataset_checked_count(vd_state);
    if (amount == 0)
    {
        return;
    }

    amount = amount * token_amount;
    snprintf(post_body, BC_URL_LEN, "{\"address\":\"%s\",\"amount\":%5.3f}", token_address, amount);
    if (curl)
    {
        int res = 0;
        char url[BC_URL_LEN];
        struct curl_slist *headers = NULL;

        snprintf(url, BC_URL_LEN, "%s:%d/%s", bc_hostname, bc_hostname_port, "fund");
        curl_easy_setopt(curl, CURLOPT_URL, url);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_fund_tokens_response);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Failed to fund: %s, reason: %s\n", token_address, curl_easy_strerror(res));
        }
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
    char tmp_str[BC_INFO_STRING_LEN] = "";

    jsmn_init(&parser);
    num_of_tokens = jsmn_parse(&parser, response, bufferlen, tokens, BC_TOK_ARRAY_SIZE);

    for (int i = 0, tok_len = 0; i < num_of_tokens; i++)
    {
        tok_len = tokens[i].end - tokens[i].start;
        int tok_len_2 = -1;
        if (strncmp("error", response + tokens[i].start, min(tok_len, strlen("error"))) == 0)
        {
            tok_len_2 = tokens[i+1].end - tokens[i+1].start;
            if (strncmp("false", response + tokens[i+1].start, min(tok_len_2, strlen("false"))) == 0)
            {
                continue;
            }
        }

        if (strncmp("message", response + tokens[i].start, min(tok_len, strlen("message"))) == 0)
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
    strncpy(token_address, new_token_address, BC_MAX_STR_LEN);
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
    strncpy(bc_hostname, new_bc_hostname, BC_MAX_STR_LEN);
}

void BlockchainDaemon_set_bc_hostname_port(int port)
{
    bc_hostname_port = port;
}
