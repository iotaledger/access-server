/*
 * This file is part of the IOTA Access distribution
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

/****************************************************************************
 * \project IOTA Access
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
#include "auth_logger.h"
#include "crypto_logger.h"
#include "network_logger.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "auth_helper.h"
#include "config_manager.h"
#include "globals_declarations.h"
#include "json_helper.h"
#include "pap.h"
#include "pap_plugin.h"
#include "pep.h"
#include "pip.h"
#include "policy_updater.h"
#include "utils.h"

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
  auth_ctx_t session;
  int state;
  int DAC_AUTH;
  char send_buffer[SEND_BUFF_LEN];

  unsigned short port;
  int end;

  int listenfd;
  int connfd;
} network_ctx_internal_t;

static void *network_thread_function(void *ptr);

int network_init(network_ctx_t *network_context) {
  network_ctx_internal_t *ctx = malloc(sizeof(network_ctx_internal_t));

  config_manager_init("config.ini");
  int tcp_port;
  if (CONFIG_MANAGER_OK != config_manager_get_option_int("network", "tcp_port", &tcp_port)) {
    ctx->port = 9998;
  } else {
    ctx->port = tcp_port;
  }

  ctx->state = 0;
  ctx->DAC_AUTH = 1;
  ctx->end = 0;
  ctx->listenfd = 0;
  ctx->connfd = 0;

  policyupdater_init();

  *network_context = (void *)ctx;

  logger_helper_init(LOGGER_INFO);
  logger_init_network(LOGGER_INFO);
  logger_init_auth(LOGGER_INFO);
  logger_init_crypto(LOGGER_INFO);

  return 0;
}

int network_start(network_ctx_t network_context) {
  network_ctx_internal_t *ctx = (network_ctx_internal_t *)network_context;

  struct sockaddr_in serv_addr;
  char read_buffer[READ_BUFF_LEN];

  ctx->listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(read_buffer, '0', sizeof(read_buffer));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(ctx->port);

  int retstat = bind(ctx->listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (retstat != 0) {
    log_error(network_logger_id, "[%s:%d] bind failed.\n", __func__, __LINE__);
    free(ctx);
    return ERROR_BIND_FAILED;
  }

  if (ctx->end != 1) {
    retstat = listen(ctx->listenfd, CONNECTION_BACKLOG_LEN);
    if (retstat != 0) {
      log_error(network_logger_id, "[%s:%d] listen failed.\n", __func__, __LINE__);
      free(ctx);
      return ERROR_LISTEN_FAILED;
    }
  }

  if (pthread_create(&ctx->thread, NULL, network_thread_function, ctx)) {
    log_error(network_logger_id, "[%s:%d] error creating thread.\n", __func__, __LINE__);
    free(ctx);
    return ERROR_CREATE_THREAD_FAILED;
  }

  return NO_ERROR;
}

void network_stop(network_ctx_t network_context) {
  network_ctx_internal_t *ctx = (network_ctx_internal_t *)network_context;
  if (ctx != NULL) {
    ctx->end = 1;
    pthread_join(ctx->thread, NULL);
    free(ctx);
  }
}

static ssize_t read_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return read(*sockfd, data, len);
}

static ssize_t write_socket(void *ext, void *data, unsigned short len) {
  int *sockfd = (int *)ext;
  return write(*sockfd, data, len);
}

// ToDo: implement proper verification.
static int verify(unsigned char *key, int len) { return 0; }

static int get_server_state(network_ctx_internal_t *ctx) { return ctx->state; }

static unsigned int calculate_decision(char **recv_data, network_ctx_internal_t *ctx) {
  int request_code = -1;
  unsigned int buffer_position = 0;

  char grant[] = "{\"response\":\"access granted\"}";
  char deny[] = "{\"response\":\"access denied \"}";
  char *msg;

  int num_of_tokens = jsonhelper_parser_init(*recv_data);

  request_code = auth_helper_check_msg_format(*recv_data);

  if (request_code == COMMAND_RESOLVE) {
    char decision[BUF_LEN] = {0};
    //@TODO: Should this be moved to access actor? Network should just send cb here to notify request.
    pep_request_access(*recv_data, (void *)decision);

    if (memcmp(decision, "grant", strlen("grant"))) {
      msg = grant;
    } else {
      msg = deny;
    }

    if (ctx->DAC_AUTH == 1) {
      free(*recv_data);
    }

    memcpy(ctx->send_buffer, msg, sizeof(grant));
    *recv_data = ctx->send_buffer;
    buffer_position = sizeof(grant);
  } else if (request_code == COMMAND_GET_POL_LIST) {
    //@TODO: Should this be moved to access actor? Network should just send cb here to notify request.
    pep_request_access(*recv_data, (void *)ctx->send_buffer);

    buffer_position = strlen(ctx->send_buffer);

    if (ctx->DAC_AUTH == 1) {
      free(*recv_data);
    }

    *recv_data = ctx->send_buffer;
  } else if (request_code == COMMAND_ENABLE_POLICY) {
    //@FIXME: Will be refactored
#if 0
        int policy_id_index = -1;

        for (int i = 0; i < num_of_tokens; i++)
        {
            if (memcmp(*recv_data + jsonhelper_get_token_start(i), "policy_id", strlen("policy_id")) == 0)
            {
                policy_id_index = i + 1;
                break;
            }
        }

        if (policy_id_index == -1)
        {
            return buffer_position;
        }

        char* pol_id_hex = calloc(jsonhelper_token_size(policy_id_index) / 2, 1);
        str_to_hex(*recv_data + jsonhelper_get_token_start(policy_id_index), pol_id_hex, jsonhelper_token_size(policy_id_index));

        int ret = -1;

        if (pol_id_hex != NULL)
        {
            ret = PolicyStore_enable_policy(pol_id_hex, jsonhelper_token_size(policy_id_index) / 2);
        }
        free(pol_id_hex);

        if (ret == 0)
        {
            memcpy(send_buffer, grant, sizeof(grant));
            *recv_data = send_buffer;
            buffer_position = sizeof(grant);
        }
        else
        {
            memcpy(send_buffer, deny, sizeof(deny));
            *recv_data = send_buffer;
            buffer_position = sizeof(deny);
        }
#endif
  } else if (request_code == COMMAND_SET_DATASET) {
    int dataset_list_index = -1;

    for (int i = 0; i < num_of_tokens; i++) {
      if (memcmp(*recv_data + jsonhelper_get_token_start(i), "dataset_list", strlen("dataset_list")) == 0) {
        dataset_list_index = i;
        break;
      }
    }

    int arr_start = dataset_list_index + 1;

    if ((dataset_list_index == -1) || (jsonhelper_get_token_at(arr_start).type != JSMN_ARRAY)) {
      memcpy(ctx->send_buffer, deny, strlen(deny));
      buffer_position = strlen(deny);
    } else {
      pip_set_dataset(*recv_data + jsonhelper_get_token_at(arr_start).start,
                      jsonhelper_get_token_at(arr_start).end - jsonhelper_get_token_at(arr_start).start);
      memcpy(ctx->send_buffer, grant, strlen(grant));
      buffer_position = strlen(grant);
    }
    *recv_data = ctx->send_buffer;
  } else if (request_code == COMMAND_GET_DATASET) {
    pip_get_dataset((char *)ctx->send_buffer, &buffer_position);
    *recv_data = ctx->send_buffer;
  } else if (request_code == COMMAND_GET_USER_OBJ) {
    char username[USERNAME_LEN] = "";

    for (int i = 0; i < num_of_tokens; i++) {
      if (memcmp(*recv_data + jsonhelper_get_token_start(i), "username", strlen("username")) == 0) {
        strncpy(username, *recv_data + jsonhelper_get_token_start(i + 1), jsonhelper_token_size(i + 1));
        username[jsonhelper_token_size(i + 1)] = '\0';
        break;
      }
    }

    log_info(network_logger_id, "[%s:%d] get user\n", __func__, __LINE__);
    pap_user_management_action(PAP_USERMNG_GET_USER, username, ctx->send_buffer);
    *recv_data = ctx->send_buffer;
    buffer_position = strlen(ctx->send_buffer);
  } else if (request_code == COMMAND_GET_USERID) {
    char username[USERNAME_LEN] = "";

    for (int i = 0; i < num_of_tokens; i++) {
      if (memcmp(*recv_data + jsonhelper_get_token_start(i), "username", strlen("username")) == 0) {
        strncpy(username, *recv_data + jsonhelper_get_token_start(i + 1), jsonhelper_token_size(i + 1));
        username[jsonhelper_token_size(i + 1)] = '\0';
        break;
      }
    }

    log_info(network_logger_id, "[%s:%d] get auth id\n", __func__, __LINE__);
    pap_user_management_action(PAP_USERMNG_GET_USER_ID, username, ctx->send_buffer);
    *recv_data = ctx->send_buffer;
    buffer_position = strlen(ctx->send_buffer);
  } else if (request_code == COMMAND_REDISTER_USER) {
    char user_data[USER_DATA_LEN];
    for (int i = 0; i < num_of_tokens; i++) {
      if (memcmp(*recv_data + jsonhelper_get_token_start(i), "user", strlen("user")) == 0) {
        strncpy(user_data, *recv_data + jsonhelper_get_token_start(i + 1), jsonhelper_token_size(i + 1));
        user_data[jsonhelper_token_size(i + 1)] = '\0';
        break;
      }
    }

    log_info(network_logger_id, "[%s:%d] put user\n", __func__, __LINE__);
    pap_user_management_action(PAP_USERMNG_PUT_USER, user_data, ctx->send_buffer);
    *recv_data = ctx->send_buffer;
    buffer_position = strlen(ctx->send_buffer);
  } else if (request_code == COMMAND_GET_ALL_USER) {
    log_info(network_logger_id, "[%s:%d] get all users\n", __func__, __LINE__);
    pap_user_management_action(PAP_USERMNG_GET_ALL_USR, ctx->send_buffer);
    *recv_data = ctx->send_buffer;
    buffer_position = strlen(ctx->send_buffer);
  } else if (request_code == COMMAND_CLEAR_ALL_USER) {
    log_info(network_logger_id, "[%s:%d] clear all users\n", __func__, __LINE__);
    pap_user_management_action(PAP_USERMNG_CLR_ALL_USR, ctx->send_buffer);
    *recv_data = ctx->send_buffer;
    buffer_position = strlen(ctx->send_buffer);
  } else {
    log_info(network_logger_id, "[%s:%d] request message format not valid\n > %s\n", __func__, __LINE__, *recv_data);
    memset(*recv_data, '0', sizeof(ctx->send_buffer));
    memcpy(ctx->send_buffer, deny, sizeof(deny));
    *recv_data = ctx->send_buffer;
    buffer_position = sizeof(deny);
  }

  return buffer_position;
}

static void *network_thread_function(void *ptr) {
  network_ctx_internal_t *ctx = (network_ctx_internal_t *)ptr;
  while (!ctx->end) {
    struct timeval tv = {0, TIME_50MS};
    int result;
    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(ctx->listenfd, &rfds);

    result = select(ctx->listenfd + 1, &rfds, (fd_set *)0, (fd_set *)0, &tv);

    if (0 < result) {
      int ret, n = 0;

      ctx->connfd = accept(ctx->listenfd, (struct sockaddr *)NULL, NULL);

      time_t now;
      struct tm ts;
      char buf[BUF_LEN];

      // Get current time
      time(&now);

      // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
      ts = *localtime(&now);
      strftime(buf, sizeof(buf), "%h:%M:%S", &ts);

      log_info(network_logger_id, "[%s:%d] Client connected.\n", __func__, __LINE__);

      // START

      if ((ctx->state == 0)) {
        ctx->state = 1;
        char *recv_data = NULL;
        unsigned short recv_len = 0;
        int auth = -1;
        int decision = -1;

        auth_init_server(&ctx->session, &ctx->connfd);

        ctx->session.f_read = read_socket;
        ctx->session.f_write = write_socket;
        ctx->session.f_verify = verify;

        auth = auth_authenticate(&ctx->session);

        if (auth == 0) {
          auth_receive(&ctx->session, (unsigned char **)&recv_data, &recv_len);
          decision = calculate_decision(&recv_data, ctx);
          auth_helper_send_decision(decision, &ctx->session, recv_data, decision);
        } else {
          time(&now);

          // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
          ts = *localtime(&now);
          strftime(buf, sizeof(buf), "%h:%M:%S", &ts);

          log_error(network_logger_id, "[%s:%d] Authentication failed.\n", __func__, __LINE__);

          decision = 0;
          int size = 34;
          write_socket(&ctx->connfd, "{\"error\":\"authentication failed\"}", size);
        }

        ctx->state = 0;

        auth_release(&ctx->session);

        unsigned char try_count = 0;
        while ((get_server_state(ctx) != 0) && (try_count++ < MAX_TRY_NUM))
          ;
      }

      // END

      close(ctx->connfd);
    }

    usleep(g_task_sleep_time);
  }
}
