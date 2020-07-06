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

#include "policy_updater.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "dlog.h"
#include "time_manager.h"
#include "utils.h"

#define RECV_BUFF_LEN 1024
#define BUFF_LEN 80

#define POLICY_UPDATER_REQ_GET_LIST_SIZE (256)

#define POLICY_UPDATER_REPLY_LIST_SIZE (1024)

#define POLICY_UPDATER_REPLY_POLICY_SIZE (2048)

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#define POLICY_UPDATER_ADDRESS_SIZE 127
#define POLICY_UPDATER_POL_ID_BUF_LEN 64
#define POLICY_UPDATER_RESPONSE_LEN 2048
#define POLICY_UPDATER_SERV_ADDR_LEN 100

static char g_policy_updater_address[POLICY_UPDATER_ADDRESS_SIZE] = "\0";
static int g_policy_updater_port = 6007;

static char g_user_address[POLICY_UPDATER_ADDRESS_SIZE] = "\0";
static int g_user_port = 9998;

static char g_module_name[] = "PolicyUpdater";

static int hostname_to_ip(const char *hostname, char *ip_address);

static ssize_t read_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return read(*sockfd, data, len);
}

static ssize_t write_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return write(*sockfd, data, len);
}

static int get_tcp_response(void *ext, char *recv_buffer) {
  int length = 0;

  int num_of_chars = read_socket(ext, recv_buffer, 1);

  while (num_of_chars == 1) {
    length++;
    num_of_chars = read_socket(ext, recv_buffer + length, 1);
  }

  recv_buffer[length] = '\0';

  length++;

  return length;
}

static int tcp_send(char *msg, int msg_length, char *rec, int *rec_length, char *servip, int port) {
  int sockfd = 0;

  char recv_buff[RECV_BUFF_LEN];
  struct sockaddr_in serv_addr;

  memset(recv_buff, '0', sizeof(recv_buff));
  if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("\n Error : Could not create socket \n");
    return 1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, servip, &serv_addr.sin_addr) <= 0) {
    return 1;
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    char buf[BUFF_LEN];

    timemanager_get_time_string(buf, BUFF_LEN);

    printf("%s %s\tError: Connection Failed\n", buf, g_module_name);

    return 1;
  }

  write_socket(&sockfd, msg, msg_length);
  *rec_length = get_tcp_response(&sockfd, rec);
  close(sockfd);

  return 0;
}

void policyupdater_get_policy(char *policy_id, char *p_policy) {
  char policy_request[POLICY_UPDATER_REQ_GET_LIST_SIZE] = {
      0,
  };

  snprintf(policy_request, POLICY_UPDATER_REQ_GET_LIST_SIZE, "{\"cmd\":\"get_policy\",\"policyId\":\"%.*s\"}",
           POLICY_UPDATER_POL_ID_BUF_LEN, policy_id);
  int response_length;
  char response[POLICY_UPDATER_RESPONSE_LEN];

  char policy_service_address[POLICY_UPDATER_SERV_ADDR_LEN];
  hostname_to_ip(g_policy_updater_address, policy_service_address);

  tcp_send(policy_request, strlen(policy_request), response, &response_length, policy_service_address,
           g_policy_updater_port);

  strncpy(p_policy, response, MIN((response_length + 1), (POLICY_UPDATER_RESPONSE_LEN - 1)));
}

void policyupdater_init() {
  configmanager_get_option_string("pap", "policy_store_service_ip", g_policy_updater_address,
                                  POLICY_UPDATER_ADDRESS_SIZE);
  configmanager_get_option_int("pap", "policy_store_service_port", &g_policy_updater_port);
  configmanager_get_option_string("pap", "user_ip", g_user_address, POLICY_UPDATER_ADDRESS_SIZE);
  configmanager_get_option_int("pap", "user_port", &g_user_port);
}

int policyupdater_start() {}

int policyupdater_stop() {}

static int hostname_to_ip(const char *hostname, char *ip_address) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  he = gethostbyname(hostname);
  if (he == NULL) {
    return 1;
  }

  addr_list = (struct in_addr **)he->h_addr_list;

  for (i = 0; addr_list[i] != NULL; i++) {
    strcpy(ip_address, inet_ntoa(*addr_list[i]));
    return 0;
  }
}

unsigned int policyupdater_get_policy_list(const char *policy_store_version, const char *device_id, char *policy_list,
                                           int *policy_list_len, int *new_policy_list_flag) {
  char policy_request[POLICY_UPDATER_REQ_GET_LIST_SIZE];
  snprintf(policy_request, POLICY_UPDATER_REQ_GET_LIST_SIZE,
           "{\"cmd\":\"get_policy_list\",\"policyStoreId\":\"%s\",\"deviceId\":\"%s\"}", policy_store_version,
           device_id);

  int response_length;
  char response[POLICY_UPDATER_RESPONSE_LEN];

  char policy_service_address[POLICY_UPDATER_SERV_ADDR_LEN];
  hostname_to_ip(g_policy_updater_address, policy_service_address);
  int res = tcp_send(policy_request, strlen(policy_request), response, &response_length, policy_service_address,
                     g_policy_updater_port);

  if (res != 1) {
    strncpy(policy_list, response, POLICY_UPDATER_RESPONSE_LEN);

    *new_policy_list_flag = 1;
    if (strlen(policy_list) >= POLICY_UPDATER_RESPONSE_LEN) {
      policy_list[POLICY_UPDATER_RESPONSE_LEN - 1] = '\0';
    }
    *policy_list_len = strlen(policy_list);
  }

  return 0;
}
