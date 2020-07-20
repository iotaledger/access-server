/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
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
 * \file datadumper.c
 * \brief
 * Data dumper and publisher interface
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"

#define DATADUMPER_STR_LEN 128
#define DATADUMPER_NAME_LEN 64
#define DATADUMPER_FILENAME_LEN 128
#define DATADUMPER_UNIX_RWX 0700

fjson_object *fj_root;

static pthread_mutex_t json_sync_lock;

static time_t json_started;

static char ipaddr[DATADUMPER_STR_LEN] = "127.0.0.1";
static char device_id[DATADUMPER_STR_LEN] = "";
static int ipport = 12345;

struct datadumper_filler_node {
  fjson_object *(*json_filler)();
  char name[DATADUMPER_NAME_LEN];
  fjson_object **added_node;
  struct datadumper_filler_node *next;
};

struct datadumper_filler_node json_filler_list = {NULL, "", NULL, NULL};

void datadumper_add_module_init_cb(fjson_object *(*json_filler)(), fjson_object **added_node, const char *name) {
  struct datadumper_filler_node *cur = &json_filler_list;

  if (cur->json_filler != NULL) {
    while (cur->next != NULL) {
      cur = cur->next;
    }

    cur->next = malloc(sizeof(struct datadumper_filler_node));
    cur->next->next = NULL;
    cur = cur->next;
  }
  cur->json_filler = json_filler;
  cur->added_node = added_node;
  strncpy(cur->name, name, DATADUMPER_NAME_LEN);
}

static void datadumper_clear_filler_node_list() {
  struct datadumper_filler_node *cur = &json_filler_list;
  struct datadumper_filler_node *tmp;

  if (cur->next == NULL) {
    return;
  }
  cur = cur->next;

  while (cur->next != NULL) {
    tmp = cur;
    cur = cur->next;
  }

  // FIXME: It seems that this doesn't do anything. Should tmp be freed
  // inside the loop, perhaps?
}

static void call_all_filler_callbacks() {
  struct datadumper_filler_node *cur = &json_filler_list;

  if (json_filler_list.json_filler == NULL || json_filler_list.added_node == NULL) {
    return;
  }

  *cur->added_node = cur->json_filler();
  fjson_object *data = fjson_object_new_object();
  fjson_object_object_add(fj_root, "data", data);
  fjson_object_object_add(fj_root, "deviceId", fjson_object_new_string(device_id));
  fjson_object_object_add(data, "timestamp", fjson_object_new_int(json_started));
  fjson_object_object_add(data, cur->name, *cur->added_node);

  while (cur->next != NULL && json_filler_list.json_filler != NULL && json_filler_list.added_node != NULL) {
    cur = cur->next;
    if (cur->json_filler == NULL || cur->added_node == NULL) {
      break;
    }

    *cur->added_node = cur->json_filler();
    fjson_object_object_add(data, cur->name, *cur->added_node);
  }
}

fjson_object *datadumper_init() {
  json_started = time(NULL);
  fj_root = fjson_object_new_object();

  configmanager_get_option_string("json_interface", "ipaddr", ipaddr, DATADUMPER_STR_LEN);
  configmanager_get_option_string("config", "device_id", device_id, DATADUMPER_STR_LEN);
  configmanager_get_option_int("json_interface", "ipport", &ipport);
  call_all_filler_callbacks();

  return fj_root;
}

void datadumper_set_address(const char *new_addr) { strncpy(ipaddr, new_addr, sizeof(ipaddr)); }

void datadumper_set_port(int new_port) { ipport = new_port; }

static void socket_send_string(const char *data) {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Socket creation failed... Not sending\n");
  }
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ipaddr);
  servaddr.sin_port = htons(ipport);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    printf("Socket connection failed.\n");
  } else {
    write(sockfd, data, strlen(data));
  }

  close(sockfd);
}

int datadumper_dump_if_needed(int dump_period_s) {
  time_t current_time = time(NULL);
  char filename[DATADUMPER_FILENAME_LEN];
  struct stat st = {0};
  int did_dump = 0;

  if (current_time > json_started + dump_period_s - 1) {
    if (pthread_mutex_trylock(&json_sync_lock) == 0) {
      if (stat("./json_log", &st) == -1) {
        mkdir("./json_log", DATADUMPER_UNIX_RWX);
      }
      snprintf(filename, DATADUMPER_FILENAME_LEN - 1, "./json_log/%ld.json", json_started);
      fjson_object_to_file_ext(filename, fj_root, FJSON_TO_STRING_PRETTY);

#if DUMP_TO_CLOUD == 1
      socket_send_string(fjson_object_to_json_string_ext(fj_root, FJSON_TO_STRING_PRETTY));
#endif

      fjson_object_put(fj_root);
      datadumper_init(NULL, -1, NULL);
      pthread_mutex_unlock(&json_sync_lock);
      did_dump = 1;
    }
  }

  return did_dump;
}

fjson_object *datadumper_get(const char *name) {
  fjson_object *retval = NULL;
  struct fjson_object_iterator fj_iter = fjson_object_iter_begin(fj_root);
  while (fj_iter.objs_remain > 0) {
    if (strncmp(fjson_object_iter_peek_name(&fj_iter), name, strlen(name)) == 0) {
      retval = fjson_object_iter_peek_value(&fj_iter);
      break;
    }
    fjson_object_iter_next(&fj_iter);
  }

  return retval;
}

void datadumper_deinit() {
  fjson_object_put(fj_root);
  datadumper_clear_filler_node_list();
}

pthread_mutex_t *datadumper_get_mutex() { return &json_sync_lock; }
