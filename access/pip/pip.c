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

/****************************************************************************
 * \project Decentralized Access Control
 * \file pip.c
 * \brief
 * Implementation of Policy Information Point
 *
 * @Author Milivoje Knezevic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 12.10.2018. Initial version.
 * 09.11.2018. Updated to parse request sent from PDP module
 * 07.05.2020. Refactoring
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pip.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "pip_plugin.h"
#include "pluginmanager.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PIP_DELIMITER_LEN 2
#define MAX_PIP_PLUGINS 5

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static pthread_mutex_t pip_mutex;

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
static pluginmanager_t plugin_manager;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
pip_error_e pip_init(void) {
  // Initalize mutex
  if (pthread_mutex_init(&pip_mutex, NULL) != 0) {
    printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
    return PIP_ERROR;
  }

  pluginmanager_init(&plugin_manager, MAX_PIP_PLUGINS);

  return PIP_NO_ERROR;
}

pip_error_e pip_term(void) {
  // stop and destroy plugins
  plugin_t* plugin;
  for (int i = 0; i < plugin_manager.plugins_num; i++) {
    pluginmanager_get(&plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_destroy(plugin);
    }
    // TODO: check status
  }

  // Destroy mutex
  pthread_mutex_destroy(&pip_mutex);

  return PIP_NO_ERROR;
}

pip_error_e pip_get_dataset(char* dataset_json, size_t* string_len) {
  plugin_t* plugin;
  for (int i = 0; i < plugin_manager.plugins_num; i++) {
    pluginmanager_get(&plugin_manager, i, &plugin);
    int callback_status = -1;
    pipplugin_string_arg_t attr;
    attr.string = dataset_json;
    attr.len = 0;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PIPPLUGIN_GET_DATASET_CB, &attr);
      *string_len = attr.len;
    }
    // TODO: check status
  }
  return PIP_NO_ERROR;
}

pip_error_e pip_set_dataset(char* dataset_json, size_t string_len) {
  plugin_t* plugin;
  for (int i = 0; i < plugin_manager.plugins_num; i++) {
    pluginmanager_get(&plugin_manager, i, &plugin);
    pipplugin_string_arg_t attr;
    attr.string = dataset_json;
    attr.len = string_len;
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PIPPLUGIN_SET_DATASET_CB, &attr);
    }
    // TODO: check status
  }
  return PIP_NO_ERROR;
}

pip_error_e pip_register_plugin(plugin_t* plugin) {
  pthread_mutex_lock(&pip_mutex);

  // Check input parameters
  if (plugin == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    pthread_mutex_unlock(&pip_mutex);
    return PIP_ERROR;
  }

  // Register callback
  if (pluginmanager_register(&plugin_manager, plugin) != 0) {
    printf("\nERROR[%s]: Callback is already registered.\n", __FUNCTION__);
    pthread_mutex_unlock(&pip_mutex);
    return PIP_ERROR;
  }

  pthread_mutex_unlock(&pip_mutex);
  return PIP_NO_ERROR;
}

pip_error_e pip_get_data(char* uri, pip_attribute_object_t* attribute) {
  char temp[PIP_MAX_STR_LEN] = {0};
  pipplugin_args_t attrs = {0};

  pthread_mutex_lock(&pip_mutex);

  // Check input parameters
  if (uri == NULL || attribute == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    pthread_mutex_unlock(&pip_mutex);
    return PIP_ERROR;
  }

  /*
      URI format looks like: authority:policy_id/type?value
      By parssing URI, authority can be obtained
      */
  if (strlen(uri) > PIP_MAX_STR_LEN) {
    printf("\nERROR[%s]: URI too long.\n", __FUNCTION__);
    pthread_mutex_unlock(&pip_mutex);
    return PIP_ERROR;
  } else {
    strncpy(attrs.uri, uri, PIP_MAX_STR_LEN);
  }

  // Fetch attribute from plugin
  plugin_t* plugin;
  for (int i = 0; i < plugin_manager.plugins_num; i++) {
    pluginmanager_get(&plugin_manager, i, &plugin);
    int ret = plugin_call(plugin, PIPPLUGIN_ACQUIRE_CB, &attrs);
    // TODO: check status
  }

  memcpy(attribute->type, attrs.attribute.type, strlen(attrs.attribute.type));
  memcpy(attribute->value, attrs.attribute.value, strlen(attrs.attribute.value));

  pthread_mutex_unlock(&pip_mutex);
  return PIP_NO_ERROR;
}

pip_error_e pip_start(void) {
  plugin_t* plugin;
  for (int i = 0; i < plugin_manager.plugins_num; i++) {
    pluginmanager_get(&plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PIPPLUGIN_START_CB, NULL);
    }
    // TODO: check status
  }
}
