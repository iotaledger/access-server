/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \file pip_plugin_gpio.c
 * \brief
 * PIP plugin for GPIO demo.
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 12.07.2020. Initial version.
 ****************************************************************************/

#include "pip_plugin_gpio.h"

static int acquire_cb(plugin_t *plugin, void *user_data) {
  if (plugin == NULL || user_data == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return -1;
  }
  pipplugin_args_t *args = (pipplugin_args_t *)user_data;
  char *uri = args->uri;
  pip_attribute_object_t attribute_object = args->attribute;

  memcpy(attribute_object.type, "boolean", strlen("boolean"));
  char *ret;
  ret = gpio_interface_read(0) ? "true" : "false";
  memcpy(attribute_object.value, ret, strlen(ret));

  return 0;
}

static int destroy_cb(plugin_t *plugin, void *not_used) {
  free(plugin->callbacks);
  return 0;
}

int pip_plugin_gpio_initializer(plugin_t *plugin, void *user_data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void *) * PIPPLUGIN_CALLBACK_COUNT);
  plugin->callbacks[PIPPLUGIN_ACQUIRE_CB] = acquire_cb;
  plugin->callbacks[PIPPLUGIN_START_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_GET_DATASET_CB] = NULL;
  plugin->callbacks[PIPPLUGIN_SET_DATASET_CB] = NULL;
  plugin->callbacks_num = PIPPLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
}
