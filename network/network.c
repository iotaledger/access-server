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
 * \file network.c
 * \brief
 * Implementation of network module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.11.2019. Initial version.
 ****************************************************************************/

#include "network.h"

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "json_parser.h"
#include "asn_auth_helper.h"
#include "pep.h"
#include "storage.h"
#include "utils_string.h"
#include "pap.h"
#include "globals_declarations.h"
#include "policy_updater.h"
#include "config_manager.h"

#define Dlog_printf printf

#define SEND_BUFF_LEN 4096
#define READ_BUFF_LEN 1025
#define BUF_LEN 80
#define CONNECTION_BACKLOG_LEN 10
#define POL_ID_HEX_LEN 32
#define POL_ID_STR_LEN 64
#define USERNAME_LEN 128
#define USER_DATA_LEN 4096
#define TIME_50MS 50000
#define MAX_TRY_NUM 350

#define NO_ERROR 0
#define ERROR_BIND_FAILED 1
#define ERROR_LISTEN_FAILED 2
#define ERROR_CREATE_THREAD_FAILED 3

#define COMMAND_RESOLVE 0
#define COMMAND_GET_POL_LIST 1
#define COMMAND_ENABLE_POLICY 2
#define COMMAND_SET_DATASET 3
#define COMMAND_GET_DATASET 4
#define COMMAND_GET_USER_OBJ 5
#define COMMAND_GET_USERID 6
#define COMMAND_REDISTER_USER 7
#define COMMAND_GET_ALL_USER 8
#define COMMAND_CLEAR_ALL_USER 9
#define COMMAND_NOTIFY_TRANSACTION 10

typedef struct {
    pthread_t thread;
    asnSession_t session;
    int state;
    int DAC_AUTH;
    char send_buffer[SEND_BUFF_LEN];

    unsigned short port;
    int end;

    int listenfd;
    int connfd;

    Dataset_state_t* ddstate;
} Network_ctx_t_;

static void *network_thread_function(void *ptr);

int Network_init(Dataset_state_t *_ddstate, Network_ctx_t* network_context)
{
    Network_ctx_t_ *ctx = malloc(sizeof(Network_ctx_t_));

    ConfigManager_init("config.ini");
    int tcp_port;
    if (CONFIG_MANAGER_OK != ConfigManager_get_option_int("network", "tcp_port", &tcp_port))
    {
        ctx->port = 9998;
    }
    else
    {
        ctx->port = tcp_port;
    }

    ctx->state = 0;
    ctx->DAC_AUTH = 1;
    ctx->end = 0;
    ctx->listenfd = 0;
    ctx->connfd = 0;
    ctx->ddstate = _ddstate;

    PolicyUpdater_init();

    *network_context = (void*)ctx;

    return 0;
}

int Network_start(Network_ctx_t network_context)
{
    Network_ctx_t_ *ctx = (Network_ctx_t_*)network_context;

    struct sockaddr_in serv_addr;
    char read_buffer[READ_BUFF_LEN];

    ctx->listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(read_buffer, '0', sizeof(read_buffer));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(ctx->port);

    int retstat = bind(ctx->listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (retstat != 0)
    {
        perror("bind failed");
        free(ctx);
        return ERROR_BIND_FAILED;
    }

    if (ctx->end != 1)
    {
        retstat = listen(ctx->listenfd, CONNECTION_BACKLOG_LEN);
        if (retstat != 0)
        {
            perror("listen failed");
            free(ctx);
            return ERROR_LISTEN_FAILED;
        }
    }

    if (pthread_create(&ctx->thread, NULL, network_thread_function, ctx))
    {
        fprintf(stderr, "Error creating thread\n");
        free(ctx);
        return ERROR_CREATE_THREAD_FAILED;
    }


    return NO_ERROR;
}

void Network_stop(Network_ctx_t network_context)
{
    Network_ctx_t_ *ctx = (Network_ctx_t_*)network_context;
    if (ctx != NULL)
    {
        ctx->end = 1;
        pthread_join(ctx->thread, NULL);
        free(ctx);
    }
}

static ssize_t read_socket(void *ext, void *data, unsigned short len)
{
    int *sockfd = (int *)ext;
    return read(*sockfd, data, len);
}

static ssize_t write_socket(void *ext, void *data, unsigned short len)
{
    int *sockfd = (int *)ext;
    return write(*sockfd, data, len);
}

static int verify(unsigned char *key, int len)
{
    return 0;
}

static int get_server_state(Network_ctx_t_ *ctx)
{
    return ctx->state;
}

static unsigned int calculate_decision(char **recvData, Network_ctx_t_ *ctx)
{
    int request_code = -1;
    unsigned int buffer_position = 0;

    char grant[] = "{\"response\":\"access granted\"}";
    char deny[] =  "{\"response\":\"access denied \"}";
    char *msg;

    int num_of_tokens = json_parser_init(*recvData);

    request_code = asnAuthHelper_check_msg_format(*recvData);

    if (request_code == COMMAND_RESOLVE)
    {
        char decision[BUF_LEN] = {0};
        //@TODO: Should this be moved to access actor? Network should just send cb here to notify request.
        PEP_request_access(*recvData, (void*)decision);

        if (memcmp(decision, "grant", strlen("grant")))
        {
            msg = grant;
        }
        else
        {
            msg = deny;
        }

        if (ctx->DAC_AUTH == 1)
        {
            free(*recvData);
        }

        memcpy(ctx->send_buffer, msg, sizeof(grant));
        *recvData = ctx->send_buffer;
        buffer_position = sizeof(grant);
    }
    else if (request_code == COMMAND_GET_POL_LIST)
    {
        //@TODO: Should this be moved to access actor? Network should just send cb here to notify request.
        PEP_request_access(*recvData, (void*)ctx->send_buffer);

        buffer_position = strlen(ctx->send_buffer);

        if (ctx->DAC_AUTH == 1)
        {
            free(*recvData);
        }

        *recvData = ctx->send_buffer;
    }
    else if (request_code == COMMAND_ENABLE_POLICY)
    {
        //@FIXME: Will be refactored
#if 0
        int policy_id_index = -1;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recvData + get_start_of_token(i), "policy_id", strlen("policy_id")) == 0)
            {
                policy_id_index = i + 1;
                break;
            }
        }

        if (policy_id_index == -1)
        {
            return buffer_position;
        }

        char* pol_id_hex = calloc(get_size_of_token(policy_id_index) / 2, 1);
        str_to_hex(*recvData + get_start_of_token(policy_id_index), pol_id_hex, get_size_of_token(policy_id_index));

        int ret = -1;

        if (pol_id_hex != NULL)
        {
            ret = PolicyStore_enable_policy(pol_id_hex, get_size_of_token(policy_id_index) / 2);
        }
        free(pol_id_hex);

        if (ret == 0)
        {
            memcpy(send_buffer, grant, sizeof(grant));
            *recvData = send_buffer;
            buffer_position = sizeof(grant);
        }
        else
        {
            memcpy(send_buffer, deny, sizeof(deny));
            *recvData = send_buffer;
            buffer_position = sizeof(deny);
        }
#endif
    }
    else if (request_code == COMMAND_SET_DATASET)
    {
        int dataset_list_index = -1;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recvData + get_start_of_token(i), "dataset_list", strlen("dataset_list")) == 0)
            {
                dataset_list_index = i;
                break;
            }
        }

        int arr_start = dataset_list_index + 1;

        if ((dataset_list_index == -1) || (get_token_at(arr_start).type != JSMN_ARRAY))
        {
            memcpy(ctx->send_buffer, deny, strlen(deny));
            buffer_position = strlen(deny);
        }
        else
        {
            Dataset_from_json(ctx->ddstate, *recvData + get_token_at(arr_start).start, get_token_at(arr_start).end - get_token_at(arr_start).start);
            memcpy(ctx->send_buffer, grant, strlen(grant));
            buffer_position = strlen(grant);
        }
        *recvData = ctx->send_buffer;
    }
    else if (request_code == COMMAND_GET_DATASET)
    {
        buffer_position = Dataset_to_json(ctx->ddstate, (char *)ctx->send_buffer);
        *recvData = ctx->send_buffer;
    }
    else if (request_code == COMMAND_GET_USER_OBJ)
    {
        char username[USERNAME_LEN] = "";

        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "username", strlen("username")) == 0)
        {
            strncpy(username, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            username[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("get user\n");
        PAP_user_management_action(PAP_USERMNG_GET_USER, username, ctx->send_buffer);
        *recvData = ctx->send_buffer;
        buffer_position = strlen(ctx->send_buffer);
    }
    else if (request_code == COMMAND_GET_USERID)
    {
        char username[USERNAME_LEN] = "";

        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "username", strlen("username")) == 0)
        {
            strncpy(username, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            username[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("get_auth_id\n");
        PAP_user_management_action(PAP_USERMNG_GET_USER_ID, username, ctx->send_buffer);
        *recvData = ctx->send_buffer;
        buffer_position = strlen(ctx->send_buffer);
    }
    else if (request_code == COMMAND_REDISTER_USER)
    {
        char user_data[USER_DATA_LEN];
        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "user", strlen("user")) == 0)
        {
            strncpy(user_data, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            user_data[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("put user\n");
        PAP_user_management_action(PAP_USERMNG_PUT_USER, user_data, ctx->send_buffer);
        *recvData = ctx->send_buffer;
        buffer_position = strlen(ctx->send_buffer);
    }
    else if (request_code == COMMAND_GET_ALL_USER)
    {
        printf("get all users\n");
        PAP_user_management_action(PAP_USERMNG_GET_ALL_USR, ctx->send_buffer);
        *recvData = ctx->send_buffer;
        buffer_position = strlen(ctx->send_buffer);
    }
    else if (request_code == COMMAND_CLEAR_ALL_USER)
    {
        printf("clear all users\n");
        PAP_user_management_action(PAP_USERMNG_CLR_ALL_USR, ctx->send_buffer);
        *recvData = ctx->send_buffer;
        buffer_position = strlen(ctx->send_buffer);
    }
    else
    {
        Dlog_printf("\nRequest message format not valid\n > %s\n", *recvData);
        memset(*recvData, '0', sizeof(ctx->send_buffer));
        memcpy(ctx->send_buffer, deny, sizeof(deny));
        *recvData = ctx->send_buffer;
        buffer_position = sizeof(deny);
    }

    return buffer_position;
}

static void *network_thread_function(void *ptr)
{
    Network_ctx_t_ *ctx = (Network_ctx_t_*)ptr;
    while (!ctx->end)
    {
        struct timeval tv = { 0, TIME_50MS };
        int result;
        fd_set rfds;

        FD_ZERO(&rfds);
        FD_SET(ctx->listenfd, &rfds);

        result = select(ctx->listenfd + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);

        if (0 < result)
        {
            int ret, n = 0;

            ctx->connfd = accept(ctx->listenfd, (struct sockaddr*) NULL, NULL);

            time_t     now;
            struct tm  ts;
            char       buf[BUF_LEN];

            // Get current time
            time(&now);

            // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
            ts = *localtime(&now);
            strftime(buf, sizeof(buf), "%H:%M:%S", &ts);

            printf("\n%s <Network status>\tClient connected", buf);

            //START

            if((ctx->state == 0))
            {
                ctx->state = 1;
                char *recvData = NULL;
                unsigned short recv_len = 0;
                int auth = -1;
                int decision = -1;

                asnAuth_init_server(&ctx->session, &ctx->connfd);

                ctx->session.f_read = read_socket;
                ctx->session.f_write = write_socket;
                ctx->session.f_verify = verify;

                auth = asnAuth_authenticate(&ctx->session);

                if(auth == 0)
                {
                    asnAuth_receive(&ctx->session, (unsigned char**)&recvData, &recv_len);
                    decision = calculate_decision(&recvData, ctx);
                    asnAuthHelper_send_decision(decision, &ctx->session, recvData, decision);
                }
                else
                {
                    time(&now);

                    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
                    ts = *localtime(&now);
                    strftime(buf, sizeof(buf), "%H:%M:%S", &ts);

                    printf("\n%s <Network status>\tError: Authentication failed\n", buf);

                    decision = 0;
                    int size = 34;
                    write_socket(&ctx->connfd, "{\"error\":\"authentication failed\"}", size);
                }

                ctx->state = 0;

                asnAuth_release(&ctx->session);

                unsigned char try = 0;
                while((get_server_state(ctx) != 0) && ( try++ < MAX_TRY_NUM));
            }

            // END

            close(ctx->connfd);
        }

        usleep(g_task_sleep_time);
    }
}
