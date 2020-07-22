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
* \file pip_plugin_obdii.c
* \brief
* PIP Plugin for OBDii
*
* @Author Djordje Golubovic, Bernardo Araujo
*
* \notes
*
* \history
* 04.03.2020. Initial version.
* 15.07.2020. Renaming
 ****************************************************************************/

#include "pip_plugin_obdii.h"

#include <errno.h>
#include <string.h>
#include "OBDIICommunication.h"
#include "data_dumper.h"

#define OBDII_NO_ERROR 0
#define OBDII_ERROR -2
#define OBDII_JSON_NAME "obdii_data"
#define OBDII_PORTNAME_LEN 64
#define OBDII_TX_ID 0x7E0
#define OBDII_RX_ID 0x7E8

typedef struct {
  char portname[OBDII_PORTNAME_LEN];
  fjson_object* fj_root;
  pthread_mutex_t* json_mutex;
} obdiireceiver_thread_args_t;

static OBDIISocket s;
static OBDIICommandSet supportedCommands;
static obdiireceiver_thread_args_t targs;
static fjson_object* fj_obj_obdii;

static fjson_object* obdii_json_filler() {
  fj_obj_obdii = fjson_object_new_array();
  return fj_obj_obdii;
}

int obdiireceiver_start();
void obdiireceiver_stop();

void obdiireceiver_init(const char* portname, pthread_mutex_t* json_mutex) {
  if (OBDIIOpenSocket(&s, portname, OBDII_TX_ID, OBDII_RX_ID, 0) < 0) {
    printf("Error opening socket: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  supportedCommands = OBDIIGetSupportedCommands(&s);

  strncpy(targs.portname, portname, OBDII_PORTNAME_LEN);

  datadumper_add_module_init_cb(obdii_json_filler, &fj_obj_obdii, OBDII_JSON_NAME);

  targs.json_mutex = datadumper_get_mutex();
}

static int start_cb(plugin_t* plugin, void* data) {
  obdiireceiver_start();
  return 0;
}

static int destroy_cb(plugin_t* plugin, void* data) {
  obdiireceiver_stop();
  return 0;
}

int pip_plugin_obdii_initializer(plugin_t* plugin, void* data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PIP_PLUGIN_CALLBACK_COUNT);
  plugin->callbacks[PIP_PLUGIN_START_CB] = start_cb;
  plugin->callbacks[PIP_PLUGIN_ACQUIRE_CB] = NULL;
  plugin->callbacks[PIP_PLUGIN_GET_DATASET_CB] = NULL;
  plugin->callbacks[PIP_PLUGIN_SET_DATASET_CB] = NULL;
  plugin->callbacks_num = PIP_PLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;

  obdiireceiver_init("can0", NULL);

  return 0;
}

static int end_thread = 0;
static pthread_t obdii_thread = 0;

static void* obdii_thread_loop(void* ptr) {
  while (end_thread == 0) {
  }
}

int obdiireceiver_start() {
  if (pthread_create(&obdii_thread, NULL, obdii_thread_loop, (void*)&targs)) {
    fprintf(stderr, "Error creating OBDII thread\n");
    return OBDII_ERROR;
  }

  return OBDII_NO_ERROR;
}

void obdiireceiver_stop() {
  end_thread = 1;
  OBDIICommandSetFree(&supportedCommands);
  OBDIICloseSocket(&s);
}
