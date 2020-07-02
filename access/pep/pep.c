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
 * \file pep.c
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 * 18.02.2019. Added enforce_request_action from pep_plugin module.
 * 21.02.2020. Added obligations functionality.
 * 11.05.2020. Refactoring
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pep.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdp.h"
#include "pep_plugin.h"
#include "pluginmanager.h"
#include "utils.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PEP_POL_ID_SIZE 32
#define PEP_ACTION_LEN 15
#define PEP_IOTA_ADDR_LEN 81
#define PEP_POL_ID_HEX_LEN 32
#define PEP_POL_ID_STR_LEN 64

#define PEP_ASCII_SPACE 32
#define PEP_ASCII_TAB 9
#define PEP_ASCII_CR 13
#define PEP_ASCII_LF 10

#define PEP_CHECK_WHITESPACE(x) \
  ((x == PEP_ASCII_SPACE) || (x == PEP_ASCII_TAB) || (x == PEP_ASCII_CR) || (x == PEP_ASCII_LF) ? TRUE : FALSE)

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static pthread_mutex_t pep_mutex;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static int normalize_request(char *request, int request_len, char **request_normalized) {
  char temp[request_len];
  int charCnt = 0;

  // Check input parameters
  if (request == NULL || request_len == 0) {
    printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
    return 0;
  }

  // Normalize request object and save it to temp
  memset(temp, 0, request_len);

  for (int i = 0; i < request_len; i++) {
    if (!PEP_CHECK_WHITESPACE(request[i])) {
      temp[charCnt] = request[i];
      charCnt++;
    }
  }

  // Allocate memory for request_normalized
  if (*request_normalized) {
    free(*request_normalized);
    *request_normalized = NULL;
  }

  *request_normalized = malloc(charCnt * sizeof(char));
  if (*request_normalized == NULL) {
    return 0;
  }

  // Copy temp to request_normalized
  memcpy(*request_normalized, temp, charCnt * sizeof(char));

  return charCnt;
}

static int append_action_item_to_str(char *str, int pos, pap_action_list_t *action_item) {
  if (action_item == NULL) {
    return 0;
  } else if (action_item->policy_id_str == NULL) {
    return 0;
  }

  int buffer_position = pos;

  if (buffer_position != 1) {
    str[buffer_position++] = ',';
  }

  str[buffer_position++] = '{';

  // add "policy_id"
  memcpy(str + buffer_position, "\"policy_id\":\"", strlen("\"policy_id\":\""));
  buffer_position += strlen("\"policy_id\":\"");

  // add "policy_id" value
  hex_to_str(action_item->policy_id_str, str + buffer_position, PEP_POL_ID_HEX_LEN);
  buffer_position += PEP_POL_ID_STR_LEN;
  str[buffer_position++] = '\"';

  // add "action"
  memcpy(str + buffer_position, ",\"action\":\"", strlen(",\"action\":\""));
  buffer_position += strlen(",\"action\":\"");

  // add "action" value
  memcpy(str + buffer_position, action_item->action, strlen(action_item->action));
  buffer_position += strlen(action_item->action);
  str[buffer_position++] = '\"';

  // check if "cost" field should be added (add it if policy is not paid)
  if (action_item->is_available.is_payed == PAP_NOT_PAYED) {
    // add "cost"
    memcpy(str + buffer_position, ",\"cost\":\"", strlen(",\"cost\":\""));
    buffer_position += strlen(",\"cost\":\"");

    // add "cost" value
    memcpy(str + buffer_position, action_item->is_available.cost, strlen(action_item->is_available.cost));
    buffer_position += strlen(action_item->is_available.cost);
    str[buffer_position++] = '\"';

    // add wallet address
    memcpy(str + buffer_position, ",\"wallet address\":\"", strlen(",\"wallet address\":\""));
    buffer_position += strlen(",\"wallet address\":\"");

    // add wallet address value
    memcpy(str + buffer_position, action_item->is_available.wallet_address, PAP_WALLET_ADDR_LEN);
    buffer_position += PAP_WALLET_ADDR_LEN;
    str[buffer_position++] = '\"';
  }

  str[buffer_position++] = '}';

  return buffer_position - pos;
}

static int list_to_string(pap_action_list_t *action_list, char *output_str) {
  output_str[0] = '[';
  int buffer_position = 1;
  pap_action_list_t *action_list_temp = action_list;

  while (action_list_temp != NULL) {
    buffer_position += append_action_item_to_str(output_str, buffer_position, action_list_temp);
    action_list_temp = action_list_temp->next;
  }

  output_str[buffer_position++] = ']';
  output_str[buffer_position++] = '\0';

  return buffer_position;
}

/****************************************************************************
 * CALLBACK FUNCTIONS
 ****************************************************************************/
#define MAX_PEP_PLUGINS 5
static pluginmanager_t plugin_manager;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
bool pep_init() {
  // Initialize mutex
  if (pthread_mutex_init(&pep_mutex, NULL) != 0) {
    printf("\nERROR[%s]: Mutex init failed.\n", __FUNCTION__);
    return FALSE;
  }

  // Initialize PDP module
  if (pdp_init() == FALSE) {
    printf("\nERROR[%s]: PDP init failed.\n", __FUNCTION__);
    return FALSE;
  }

  // initialize plugin manager for pep
  pluginmanager_init(&plugin_manager, MAX_PEP_PLUGINS);

  return TRUE;
}

bool pep_term(void) {
  // Destroy mutex
  pthread_mutex_destroy(&pep_mutex);

  // Terminate PDP module
  if (pdp_term() == FALSE) {
    printf("\nERROR[%s]: PDP term failed.\n", __FUNCTION__);
    return FALSE;
  }

  return TRUE;
}

bool pep_register_callback(plugin_t *plugin) {
  pthread_mutex_lock(&pep_mutex);

  // Register plugin
  pluginmanager_register(&plugin_manager, plugin);

  pthread_mutex_unlock(&pep_mutex);

  return TRUE;
}

bool pep_unregister_callback(void) {
  return TRUE;
}

bool pep_request_access(char *request, void *response) {
  char action_value[PEP_ACTION_LEN];
  // TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
  // char obligation[PDP_OBLIGATION_LEN];
  char tangle_address[PEP_IOTA_ADDR_LEN];
  char *norm_request = NULL;
  // pdp_action_t action;
  pdp_decision_e ret = PDP_ERROR;
  pepplugin_args_t plugin_args = {0};

  // Check input parameter
  if (request == NULL || response == NULL) {
    printf("\n\nERROR[%s]: Invalid input parameter.\n\n", __FUNCTION__);
    return FALSE;
  }

  pthread_mutex_lock(&pep_mutex);

  plugin_args.action.value = action_value;
  plugin_args.action.wallet_address = tangle_address;

  // Get normalized request
  if (normalize_request(request, strlen(request), &norm_request) == 0) {
    printf("\n\nERROR[%s]: Error normalizing request.\n\n", __FUNCTION__);
    pthread_mutex_unlock(&pep_mutex);
    return FALSE;
  }

  // Calculate decision and get action + obligation
  ret = pdp_calculate_decision(norm_request, plugin_args.obligation, &plugin_args.action);

  if (memcmp(plugin_args.action.value, "get_actions", strlen("get_actions"))) {
    pap_action_list_t *temp = NULL;

    list_to_string(plugin_args.action.action_list, (char *)response);

    while (plugin_args.action.action_list) {
      temp = plugin_args.action.action_list;
      plugin_args.action.action_list = plugin_args.action.action_list->next;
      free(temp);
    }
  } else {
    plugin_t *plugin = NULL;
    pluginmanager_get(&plugin_manager, 0, &plugin);
    if (plugin != NULL) {
      plugin_call(plugin, PEPPLUGIN_ACTION_CB, &plugin_args);
    }

    ret == PDP_GRANT ? memcpy(response, "grant", strlen("grant")) : memcpy(response, "deny", strlen("deny"));
  }

  free(norm_request);
  pthread_mutex_unlock(&pep_mutex);
  return TRUE;
}
