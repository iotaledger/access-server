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

#include "tcp_client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "resolver.h"

char module_name[] = "<AWS connection>";

static ssize_t read_socket(void *ext, void *data, unsigned short len)
{
   int *sockfd = (int *)ext;
   ssize_t ret = read(*sockfd, data, len);
   return ret;
}

static ssize_t write_socket(void *ext, void *data, unsigned short len)
{
   int *sockfd = (int *)ext;
   ssize_t ret = write(*sockfd, data, len);
   return ret;
}

int read_AWS_response(void *ext, char *recvBuffer)
{
    int length = 0;

    int num_of_chars = read_socket(ext, recvBuffer, 1);

    while(num_of_chars == 1)
    {
        length++;
        num_of_chars = read_socket(ext, recvBuffer + length, 1);
    }

    recvBuffer[length] = '\0';

    length++;

    return length;
}


int tcp_client_send(char *msg, int msg_length, char *rec, int *rec_length, char *servip, int port)
{
    int count = 0;
    int ret = 0;

    int sockfd = 0;

    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, servip, &serv_addr.sin_addr) <= 0)
    {
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        char buf[80];

        get_time(buf);

        printf("%s %s\tError: Connection Failed\n", buf, module_name);

        return 1;
    }

    write_socket(&sockfd, msg, msg_length);
    *rec_length = read_AWS_response(&sockfd, rec);
    close(sockfd);

    return 0;
}
int AWS_request(char *msg, int msg_length, char *rec, int *rec_length)
{}
/*
int AWS_request(char *msg, int msg_length, char *rec, int *rec_length)
{
    int count = 0;
    int ret = 0;

    int sockfd = 0;
    int port = AWS_PORT;//6003;

    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    char* servip = "52.59.214.233";

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, servip, &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        //printf("\n Error : Connect Failed \n");
    char buf[80];

    get_time(buf);

        printf("%s %s\tError: Connection Failed\n", buf, action_ps);

        return 1;
    }

    //write_socket(&sockfd, "{\"cmd\":\"get_policy_list\",\"policy_store_id\":\"0x0\"}", 49);
    write_socket2(&sockfd, msg, msg_length);
    //read_socket(&sockfd, &recvBuff, 169);
    *rec_length = read_AWS_response(&sockfd, rec);

    close(sockfd);

    return 0;

}
*/


