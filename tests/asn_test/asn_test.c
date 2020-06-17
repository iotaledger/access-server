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

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "asn_auth.h"
#include "asn_auth_helper.h"

#define RECV_BUFF_LEN 1024

int g_task_sleep_time = 1000000;

static int state = 0;
int get_server_state() { return state; }

void print_data(char *name, unsigned char *data, int len);

ssize_t read_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return read(*sockfd, data, len);
}

ssize_t write_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return write(*sockfd, data, len);
}

int verify(unsigned char *key, int len) { return 0; }

static asn_ctx_t session;

int main(int argc, char **argv) {
  int count = 0;
  int ret = 0;

  int sockfd = 0;
  int port = 9998;

  unsigned char recv_buff[RECV_BUFF_LEN];
  struct sockaddr_in serv_addr;
  char *servip = "127.0.0.1";

  memset(recv_buff, '0', sizeof(recv_buff));
  if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    printf("\n Error : Could not create socket \n");
    return 1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, servip, &serv_addr.sin_addr) <= 0) {
    printf("\n inet_pton error occured\n");
    return 1;
  }

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

  asnauth_init_client(&session, &sockfd);

  session.f_read = read_socket;
  session.f_write = write_socket;
  session.f_verify = verify;

  int auth = asnauth_authenticate(&session);

  if (argc == 2) {
    asnauthhelper_send_decision(1, &session, argv[1], strlen(argv[1]) + 1);
  } else {
    asnauthhelper_send_decision(1, &session, "{\"cmd\": \"get_all_users\"}",
                                strlen("{\"cmd\": \"get_all_users\"}") + 1);
  }

  unsigned short length;
  asnauth_receive(&session, (unsigned char **)&recv_buff, &length);

  int temp = (int)length;

  print_data("\n\n\nRECEIVED DATA: ", recv_buff, temp);

  close(sockfd);

  return 0;
}

void print_data(char *name, unsigned char *data, int len) {
  int i;
  printf("\n%s (%d) = {", name, len);
  for (i = 0; i < len - 1; i++) {
    printf("0x%02X, ", data[i]);
  }
  printf("0x%02X};\n\n", data[len - 1]);
}
