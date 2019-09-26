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
 * \file tcp_server.c
 * \brief
 * Implementation of tcp_server module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.11.2019. Initial version.
 ****************************************************************************/

#include "tcp_server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "json_parser.h"
#include "libauthdac.h"
#include "authDacHelper.h"
#include "pep.h"
#include "policystore.h"
#include "utils_string.h"
#include "user_management.h"
#include "globals_declarations.h"

#define Dlog_printf printf

static pthread_t thread;
static dacSession_t session;
static int state = 0;
static int DAC_AUTH = 1;
static char send_buffer[4096];

static unsigned short port = 9998;
static int end = 0;

static int listenfd = 0;
static int connfd = 0;

static VehicleDataset_state_t* vdstate;

static void *server_thread(void *ptr);

int TCPServer_start(int portname, VehicleDataset_state_t *_vdstate)
{
    port = portname;
    vdstate = _vdstate;

    struct sockaddr_in serv_addr;

    char read_buffer[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(read_buffer, '0', sizeof(read_buffer));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int retstat = bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (retstat != 0) {
        perror("bind failed");
        end = 1;
        return 1;
    }

    if (end != 1) {
        retstat = listen(listenfd, 10);
        if (retstat != 0) {
            perror("listen failed");
            end = 1;
            return 2;
        }
    }

    if (pthread_create(&thread, NULL, server_thread, NULL))
    {
        fprintf(stderr, "Error creating thread\n");
        return 3;
    }
    return 0;
}

void TCPServer_stop()
{
    end = 1;
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

static int get_server_state()
{
    return state;
}

static int resolve_obligation(int obligation)
{
/*
    if(1 == obligation)
    {
        printf("\nUnlock");
    }
    else if(2 == obligation)
    {
        printf("\nLock");
    }
    else
    {
    printf("\nUNDEFINED %d", obligation);
    }
*/
    return 0;
}

static int append_action_item_to_str(char *str, int pos, list_t *action_item)
{
    if(action_item == NULL)
        return 0;
    else if(action_item->policyID == NULL)
        return 0;

    int buffer_position = pos;

    if(buffer_position != 1)
        str[buffer_position++] = ',';

    str[buffer_position++] = '{';

    // add "policy_id"
    memcpy(str + buffer_position, "\"policy_id\":\"", 13);
    buffer_position += 13;

    // add "policy_id" value
    hex_to_str(action_item->policyID, str + buffer_position, 32);
    buffer_position += 64;
    str[buffer_position++] = '\"';

    // add "action"
    memcpy(str + buffer_position, ",\"action\":\"", 11);
    buffer_position += 11;

    // add "action" value
    memcpy(str + buffer_position, action_item->action, action_item->action_length);
    buffer_position += action_item->action_length;
    str[buffer_position++] = '\"';

    int is_paid = PolicyStore_is_policy_paid(action_item->policyID, 32);

    // check if "cost" field should be added (add it if policy is not paid)
    if (is_paid == 0)
    {
        // add "cost"
        memcpy(str + buffer_position, ",\"cost\":\"", 9);
        buffer_position += 9;

        // add "cost" value
        memcpy(str + buffer_position, action_item->policy_cost, action_item->policy_cost_size);
        buffer_position += action_item->policy_cost_size;
        str[buffer_position++] = '\"';
    }

    str[buffer_position++] = '}';

    return buffer_position - pos;
}

static int list_to_string(list_t *action_list, char *output_str)
{
    output_str[0] = '[';
    int buffer_position = 1;
    list_t *action_list_temp = action_list;

    while(action_list_temp != NULL)
    {
        buffer_position += append_action_item_to_str(output_str, buffer_position, action_list_temp);
        action_list_temp = action_list_temp->next;
    }

    output_str[buffer_position++] = ']';
    output_str[buffer_position++] = '\0';

    return buffer_position;
}

static unsigned int doAuthWorkTiny(char **recvData, int *obl)
{
    int request_code = -1;
    int decision = -1;
    unsigned int buffer_position = 0;

    char grant[] = "{\"response\":\"access granted\"}";
    char deny[] =  "{\"response\":\"access denied \"}";
    char *msg;

    int num_of_tokens = json_parser_init(*recvData);

    request_code = checkMsgFormat_new(*recvData);

    if(request_code == 0)
    {
        /** "resolve" command */

        decision = pep_request_access(*recvData, obl);

        if(decision == 1)
        {
            msg = grant;
        }
        else
        {
            msg = deny;
        }

        if(DAC_AUTH == 1)
        {
            free(*recvData);
        }

        memcpy(send_buffer, msg, sizeof(grant));
        *recvData = send_buffer;
        buffer_position = sizeof(grant);
    }
    else if (request_code == 1)
    {
        /** "get_policy_list" command */

        list_t *action_list = NULL;

        // index of "user_id" token
        int user_id_index = 0;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recvData + get_start_of_token(i), "user_id", 7) == 0)
            {
                user_id_index = i + 1;
                break;
            }
        }

        PolicyStore_get_list_of_actions(*recvData + get_start_of_token(user_id_index), get_size_of_token(user_id_index), &action_list);

        buffer_position = list_to_string(action_list, (char *)send_buffer);
        Dlog_printf("\nResponse: %s\n", send_buffer);

        if(DAC_AUTH == 1)
        {
            free(*recvData);
        }

        *recvData = send_buffer;

        PolicyStore_clear_list_of_actions(action_list);
    }
    else if (request_code == 2)
    {
        /** "PolicyStore_enable_policy" command */

        int policy_id_index = -1;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recvData + get_start_of_token(i), "policy_id", 9) == 0)
            {
                policy_id_index = i + 1;
                break;
            }
        }

        if (policy_id_index == -1)
            return buffer_position;

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
    }
    else if (request_code == 3)
    {
        /** "set_dataset" command */

        int dataset_list_index = -1;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recvData + get_start_of_token(i), "dataset_list", 12) == 0)
            {
                dataset_list_index = i;
                break;
            }
        }

        int arr_start = dataset_list_index + 1;

        if ((dataset_list_index == -1) || (get_token_at(arr_start).type != JSMN_ARRAY))
        {
            memcpy(send_buffer, deny, strlen(deny));
            buffer_position = strlen(deny);
        }
        else
        {
            VehicleDataset_from_json(vdstate, *recvData + get_token_at(arr_start).start, get_token_at(arr_start).end - get_token_at(arr_start).start);
            memcpy(send_buffer, grant, strlen(grant));
            buffer_position = strlen(grant);
        }
        *recvData = send_buffer;
    }
    else if (request_code == 4)
    {
        /** "get_dataset" command */
        buffer_position = VehicleDataset_to_json(vdstate, (char *)send_buffer);
        *recvData = send_buffer;
    }
    else if (request_code == 5)
    {
        char username[128] = "";

        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "username", 8) == 0)
        {
            strncpy(username, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            username[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("get user\n");
        UserManagement_get_string(username, send_buffer);
        *recvData = send_buffer;
        buffer_position = strlen(send_buffer);
    }
    else if (request_code == 6) // get_auth_user_id
    {
        char username[128] = "";

        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "username", 8) == 0)
        {
            strncpy(username, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            username[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("get_auth_id\n");
        UserManagement_get_authenteq_id(username, send_buffer);
        *recvData = send_buffer;
        buffer_position = strlen(send_buffer);
    }
    else if (request_code == 7) // register_user
    {
        char user_data[4096];
        for (int i = 0; i < num_of_tokens; i++)
        {
        if (memcmp(*recvData + get_start_of_token(i), "user", 4) == 0)
        {
            strncpy(user_data, *recvData + get_start_of_token(i+1), get_size_of_token(i+1));
            user_data[get_size_of_token(i+1)] = '\0';
            break;
        }
        }

        printf("put user\n");
        UserManagement_put_string(user_data, send_buffer);
        *recvData = send_buffer;
        buffer_position = strlen(send_buffer);
    }
    else if (request_code == 8) // get_all_users
    {
        printf("get all users\n");
        UserManagement_get_all_users(send_buffer);
        *recvData = send_buffer;
        buffer_position = strlen(send_buffer);
    }
    else if (request_code == 9) // clear_all_users
    {
        printf("clear all users\n");
        UserManagement_clear_all_users(send_buffer);
        *recvData = send_buffer;
        buffer_position = strlen(send_buffer);
    }
    else
    {
        Dlog_printf("\nRequest message format not valid\n > %s\n", *recvData);
        memset(*recvData, '0', sizeof(send_buffer));
        memcpy(send_buffer, deny, sizeof(deny));
        *recvData = send_buffer;
        buffer_position = sizeof(deny);
    }

    return buffer_position;
}

static void *server_thread(void *ptr)
{
    while (!end)
    {
        struct timeval tv = { 0, 50000 };  // 50 ms
        int result;
        fd_set rfds;

        FD_ZERO(&rfds);
        FD_SET(listenfd, &rfds);

        result = select(listenfd + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);

        if (0 < result)
        {
            int ret, n = 0;

            connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);

            time_t     now;
            struct tm  ts;
            char       buf[80];

            // Get current time
            time(&now);

            // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
            ts = *localtime(&now);
            strftime(buf, sizeof(buf), "%H:%M:%S", &ts);

            printf("\n%s <Network status>\tClient connected", buf);

            //START

            if((state == 0))
            {
                // Dlog_printf("\n2: Success");
                state = 1;
                char *recvData = NULL;
                unsigned short recv_len = 0;
                int auth = -1;
                int decision = -1;
                int obligation = -1;

                dacInitServer(&session, &connfd);

                session.f_read = read_socket;
                session.f_write = write_socket;
                session.f_verify = verify;

                auth = dacAuthenticate(&session);

                // pthread_mutex_lock(&lock);

                if(auth == 0)
                {
                    dacReceive(&session, (unsigned char**)&recvData, &recv_len);
                    decision = doAuthWorkTiny(&recvData, &obligation);
                    sendDecision_new(decision, &session, recvData, decision);
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
                    write_socket(&connfd, "{\"error\":\"authentication failed\"}", size);
                }

                state = 0;

                dacRelease(&session);

                unsigned char try = 0;
                while((get_server_state() != 0) && ( try++ < 350));
                resolve_obligation(obligation);
            }

            // pthread_mutex_unlock(&lock);
            // END

            close(connfd);
        }

        usleep(g_task_sleep_time);
    }
}
