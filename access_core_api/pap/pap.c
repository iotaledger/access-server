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
 * \project IOTA Access
 * \file pap.c
 * \brief
 * Implementation of Policy Administration Point
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 28.04.2020. Initial version.
 ****************************************************************************/

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pap.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "apiorig.h"
#include "pap_logger.h"
#include "pap_plugin.h"
#include "plugin_manager.h"
#include "sha256.h"
#include "utils.h"
#include "validator.h"

#define JSMN_HEADER
#include "jsmn.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define PAP_MAX_TOKENS 1024
#define PAP_ASCII_SPACE 32
#define PAP_ASCII_TAB 9
#define PAP_ASCII_CR 13
#define PAP_ASCII_LF 10

#define PAP_CHECK_WHITESPACE(x) \
  ((x == PAP_ASCII_SPACE) || (x == PAP_ASCII_TAB) || (x == PAP_ASCII_CR) || (x == PAP_ASCII_LF) ? TRUE : FALSE)

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static unsigned char public_key[PAP_PUBLIC_KEY_LEN];
static unsigned char private_key[PAP_PRIVATE_KEY_LEN];
static pthread_mutex_t pap_mutex;
static plugin_manager_t g_plugin_manager;

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static int normalize_JSON_object(char *json_object, int object_len, char **json_object_normalized) {
  char temp[object_len];
  int charCnt = 0;

  // Check input parameters
  if (json_object == NULL || object_len == 0) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return 0;
  }

  // Normalize JSON object and save it to temp
  memset(temp, 0, object_len);

  for (int i = 0; i < object_len; i++) {
    if (!PAP_CHECK_WHITESPACE(json_object[i])) {
      temp[charCnt] = json_object[i];
      charCnt++;
    }
  }

  // Allocate memory for json_object_normalized
  if (*json_object_normalized) {
    free(*json_object_normalized);
    *json_object_normalized = NULL;
  }

  *json_object_normalized = malloc(charCnt * sizeof(char));
  if (*json_object_normalized == NULL) {
    return 0;
  }

  // Copy temp to json_object_normalized
  memcpy(*json_object_normalized, temp, charCnt * sizeof(char));

  return charCnt;
}

static void get_SHA256_hash(char *msg, int msg_len, char *hash_val) {
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, msg, msg_len);
  sha256_final(&ctx, hash_val);
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
pap_error_e pap_init(void) {
  plugin_manager_init(&g_plugin_manager, PAP_PLUGIN_MAX_COUNT);

  // Generate keypair
  crypto_sign_keypair(public_key, private_key);

  // Init User Management
  user_init();

  // Initalize mutex
  if (pthread_mutex_init(&pap_mutex, NULL) != 0) {
    log_error(pap_logger_id, "[%s:%d] Mutex init failed.\n", __func__, __LINE__);
    return PAP_ERROR;
  }

  policyloader_start();

  return PAP_NO_ERROR;
}

pap_error_e pap_term(void) {
  policyloader_stop();

  // Term User Management
  user_deinit();

  // Destroy mutex
  pthread_mutex_destroy(&pap_mutex);

  return PAP_NO_ERROR;
}

pap_error_e pap_register_plugin(plugin_t *plugin) {
  pthread_mutex_lock(&pap_mutex);

  plugin_manager_register(&g_plugin_manager, plugin);

  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

pap_error_e pap_add_policy(char *signed_policy, int signed_policy_size, char *parsed_policy_id, char *owner_public_key) {
  char signature[PAP_SIGNATURE_LEN];
  char policy_id[PAP_POL_ID_MAX_LEN + 1] = {0};
  char policy_obj_hash[PAP_POL_ID_MAX_LEN + 1] = {0};
  char signed_policy_id[PAP_SIGNATURE_LEN + PAP_POL_ID_MAX_LEN + 1] = {0};
  char *policy = NULL;
  char *policy_object_buff = NULL;
  char *policy_object_norm = NULL;
  int policy_object_len = 0;
  int policy_object_norm_len = 0;
  int tok_num = 0;
  unsigned long long policy_size = 0;
  unsigned long long smlen;
  jsmn_parser parser;
  jsmntok_t tokens[PAP_MAX_TOKENS];
  validator_report_t report;
  pap_policy_t put_args = {0};

  // Check input parameters (parsed_policy_id is optional)
  if (signed_policy == NULL || signed_policy_size == 0) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return PAP_ERROR;
  }

  pthread_mutex_lock(&pap_mutex);

#ifndef PAP_SIGNED_POLICY_ALL
  policy_size = signed_policy_size - PAP_SIGNATURE_LEN;
  policy = malloc((policy_size + 1) * sizeof(char));
  memset(policy, 0, policy_size * sizeof(char));
  memcpy(signature, signed_policy, PAP_SIGNATURE_LEN);
  memcpy(policy, &signed_policy[PAP_SIGNATURE_LEN], policy_size);
#else
  // Verify policy signature
  policy = malloc(signed_policy_size * sizeof(char));  // Worst case scenario
  memset(policy, 0, signed_policy_size * sizeof(char));
  if (crypto_sign_open(policy, &policy_size, signed_policy, signed_policy_size, owner_public_key) != 0) {
    // Signature verification failed
    printf("\nERROR[%s]: Policy signature can not be verified.\n", __FUNCTION__);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }
#endif

  // Check policy validity
  memset(&report, 0, sizeof(validator_report_t));
  validator_check(policy, &report);
  if ((report.valid_json == 0) || (report.proper_format == 0)) {
    log_error(pap_logger_id, "[%s:%d] Invalid policy.\n", __func__, __LINE__);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  // Parse policy
  jsmn_init(&parser);

  tok_num = jsmn_parse(&parser, policy, policy_size, tokens, PAP_MAX_TOKENS);
  if (tok_num <= 0) {
    log_error(pap_logger_id, "[%s:%d] Parsing policy failed.\n", __func__, __LINE__);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  for (int i = 0; i < tok_num; i++) {
    // Get policy_id
    if (strncmp(&policy[tokens[i].start], "policy_id", strlen("policy_id")) == 0) {
      if ((tokens[i + 1].end - tokens[i + 1].start) <= PAP_POL_ID_MAX_LEN * 2) {
#ifndef PAP_SIGNED_POLICY_ALL
        char temp[PAP_POL_ID_MAX_LEN * 2 + PAP_SIGNATURE_LEN] = {0};
        char verified_policy_id[PAP_POL_ID_MAX_LEN * 2];
        unsigned long long verified_policy_id_len;

        memcpy(temp, signature, PAP_SIGNATURE_LEN);
        memcpy(&temp[PAP_SIGNATURE_LEN], &policy[tokens[i + 1].start], PAP_POL_ID_MAX_LEN * 2);

        //Verify signature
        if (crypto_sign_open(verified_policy_id, &verified_policy_id_len, temp, PAP_POL_ID_MAX_LEN * 2 + PAP_SIGNATURE_LEN, owner_public_key) != 0) {
          // Signature verification failed
          log_error(pap_logger_id, "[%s:%d] Policy signature can not be verified.\n", __func__, __LINE__);
          free(policy);
          pthread_mutex_unlock(&pap_mutex);
          return PAP_ERROR;
        }
#endif
        if (parsed_policy_id) {
          memcpy(parsed_policy_id, &policy[tokens[i + 1].start], (tokens[i + 1].end - tokens[i + 1].start));
        }

        if (str_to_hex(&policy[tokens[i + 1].start], put_args.policy_id, (tokens[i + 1].end - tokens[i + 1].start)) !=
            UTILS_STRING_SUCCESS) {
          log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
          free(policy);
          pthread_mutex_unlock(&pap_mutex);
          return PAP_ERROR;
        }

        // If policy with this ID is already in the storage, abort the action
        plugin_t *plugin = NULL;
        pap_plugin_has_args_t has_args;
        has_args.policy_id = put_args.policy_id;
        int callback_fired = 0;
        for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
          plugin_manager_get(&g_plugin_manager, i, &plugin);
          int callback_status = -1;
          if (plugin != NULL) {
            callback_status = plugin_call(plugin, PAP_PLUGIN_HAS_CB, &has_args);
            callback_fired = 1;
          }
          // TODO: check status
        }

        if (callback_fired == 0) {
          log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_HAS_CB.\n", __func__, __LINE__);
          free(policy);
          pthread_mutex_unlock(&pap_mutex);
          return PAP_ERROR;
        }

        if (has_args.does_have == 1) {
          log_error(pap_logger_id, "[%s:%d] This policy is already stored.\n", __func__, __LINE__);
          free(policy);
          pthread_mutex_unlock(&pap_mutex);
          return PAP_ERROR;
        }
      } else {
        log_error(pap_logger_id, "[%s:%d] Size of policy id does not match supported hash functions.\n", __func__, __LINE__);
        free(policy);
        pthread_mutex_unlock(&pap_mutex);
        return PAP_ERROR;
      }
    }

    // Get policy_object
    if (strncmp(&policy[tokens[i].start], "policy_object", strlen("policy_object")) == 0) {
      policy_object_len = tokens[i + 1].end - tokens[i + 1].start;
      policy_object_buff = malloc(policy_object_len * sizeof(char));
      memcpy(policy_object_buff, &policy[tokens[i + 1].start], policy_object_len);

      // Policy object should be normalized
      policy_object_norm_len = normalize_JSON_object(policy_object_buff, policy_object_len, &policy_object_norm);

      put_args.policy_object.policy_object = policy_object_norm;
      put_args.policy_object.policy_object_size = policy_object_norm_len;
    }

    // Get policy cost
    if (strncmp(&policy[tokens[i].start], "cost", strlen("cost")) == 0) {
      memset(put_args.policy_object.cost, 0, PAP_MAX_COST_LEN * sizeof(char));
      memcpy(put_args.policy_object.cost, &policy[tokens[i + 1].start], (tokens[i + 1].end - tokens[i + 1].start));
    }

    // Get hash_fn
    if (strncmp(&policy[tokens[i].start], "hash_function", strlen("hash_function")) == 0) {
      if (strncmp(&policy[tokens[i + 1].start], "sha-256", strlen("sha-256")) == 0) {
        put_args.hash_function = PAP_SHA_256;
      } else {
        log_error(pap_logger_id, "[%s:%d] Hash function not supported.\n", __func__, __LINE__);
        free(policy_object_buff);
        free(policy_object_norm);
        free(policy);
        pthread_mutex_unlock(&pap_mutex);
        return PAP_ERROR;
      }
    }
  }

  // Check if policy ID responds to policy object
  get_SHA256_hash(policy_object_buff, policy_object_len, policy_obj_hash);
  free(policy_object_buff);

  if (memcmp(put_args.policy_id, policy_obj_hash, PAP_POL_ID_MAX_LEN * sizeof(char)) != 0) {
    log_error(pap_logger_id, "[%s:%d] Policy ID is not coresponding to policy object.\n", __func__, __LINE__);
    free(policy_object_norm);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  // Signe policy ID and save signature, this will be use in order to verify policy on acquisition from storage
  crypto_sign(signed_policy_id, &smlen, put_args.policy_id, PAP_POL_ID_MAX_LEN, private_key);

  memset(put_args.policy_id_signature.public_key, 0, PAP_PUBLIC_KEY_LEN * 2 * sizeof(char));
  memset(put_args.policy_id_signature.signature, 0, PAP_SIGNATURE_LEN * 2 * sizeof(char));

  if (hex_to_str(public_key, put_args.policy_id_signature.public_key, PAP_PUBLIC_KEY_LEN) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert hex value to string.\n", __func__, __LINE__);
    free(policy_object_norm);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  if (hex_to_str(signed_policy_id, put_args.policy_id_signature.signature, PAP_SIGNATURE_LEN) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert hex value to string.\n", __func__, __LINE__);
    free(policy_object_norm);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  put_args.policy_id_signature.signature_algorithm = PAP_ECDSA;

  // Put policy in storage
  plugin_t *plugin = NULL;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_PUT_CB, &put_args);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_PUT_CB.\n", __func__, __LINE__);
    free(policy_object_norm);
    free(policy);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  free(policy_object_norm);
  free(policy);
  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

pap_error_e pap_get_policy(char *policy_id, int policy_id_len, pap_policy_t *policy) {
  char calc_policy_id[PAP_POL_ID_MAX_LEN + 1] = {0};
  char signed_policy_id[PAP_SIGNATURE_LEN + PAP_POL_ID_MAX_LEN + 1] = {0};
  char recovered_signature[PAP_SIGNATURE_LEN + 1] = {0};
  unsigned long long smlen;
  pap_plugin_get_args_t get_args = {0};
  get_args.policy = policy;

  // Check input parameters
  if (policy_id == NULL || policy_id_len == 0 || policy == NULL) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return PAP_ERROR;
  }

  pthread_mutex_lock(&pap_mutex);

  // Get policy ID hex value
  if (str_to_hex(policy_id, get_args.policy_id, policy_id_len) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  // Get policy from storage
  plugin_t *plugin = NULL;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_GET_CB, &get_args);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  // Check if policy_id_signature is valid
  if (policy->policy_id_signature.signature_algorithm == PAP_ECDSA) {
    // Calculate policy object hash
    if (policy->hash_function == PAP_SHA_256) {
      get_SHA256_hash(policy->policy_object.policy_object, policy->policy_object.policy_object_size, calc_policy_id);

      // Sign policy ID with module private key
      crypto_sign(signed_policy_id, &smlen, calc_policy_id, PAP_POL_ID_MAX_LEN, private_key);

      if (str_to_hex(policy->policy_id_signature.signature, recovered_signature, PAP_SIGNATURE_LEN * 2) != UTILS_STRING_SUCCESS) {
        log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
        pthread_mutex_unlock(&pap_mutex);
        return PAP_ERROR;
      }

      // Check if that signature matches with acquired one
      if (memcmp(signed_policy_id, recovered_signature, PAP_SIGNATURE_LEN) != 0) {
        log_error(pap_logger_id, "[%s:%d] Invalid policy ID signature.\n", __func__, __LINE__);
        pthread_mutex_unlock(&pap_mutex);
        return PAP_ERROR;
      }
    } else {
      log_error(pap_logger_id, "[%s:%d] Invalid policy ID signature.\n", __func__, __LINE__);
      pthread_mutex_unlock(&pap_mutex);
      return PAP_ERROR;
    }
  } else {
    log_error(pap_logger_id, "[%s:%d] Invalid policy ID signature.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

bool pap_has_policy(char *policy_id, int policy_id_len) {
  char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

  // Check input parameters
  if (policy_id == NULL || policy_id_len == 0) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return FALSE;
  }

  pthread_mutex_lock(&pap_mutex);

  // Get policy ID hex value
  if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return FALSE;
  }

  // Check if policy is already in the storage
  plugin_t *plugin = NULL;
  pap_plugin_has_args_t has_args;
  has_args.policy_id = policy_id_hex;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_HAS_CB, &has_args);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_HAS_CB.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  if (has_args.does_have == 1) {
    log_error(pap_logger_id, "[%s:%d] This policy is already stored.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }
}

pap_error_e pap_remove_policy(char *policy_id, int policy_id_len) {
  char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

  // Check input parameters
  if (policy_id == NULL || policy_id_len == 0) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return PAP_ERROR;
  }

  pthread_mutex_lock(&pap_mutex);

  // Get policy ID hex value
  if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  // Delete policy from storage
  plugin_t *plugin = NULL;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_DEL_CB, policy_id_hex);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

pap_error_e pap_get_policy_obj_len(char *policy_id, int policy_id_len, int *pol_obj_len) {
  char policy_id_hex[PAP_POL_ID_MAX_LEN + 1] = {0};

  // Check input parameters
  if (policy_id == NULL || policy_id_len == 0 || pol_obj_len == NULL) {
    printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
    return PAP_ERROR;
  }

  pthread_mutex_lock(&pap_mutex);

  // Get policy ID hex value
  if (str_to_hex(policy_id, policy_id_hex, policy_id_len) != UTILS_STRING_SUCCESS) {
    log_error(pap_logger_id, "[%s:%d] Could not convert string to hex value.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  plugin_t *plugin = NULL;
  pap_plugin_len_args_t args;
  args.policy_id = policy_id_hex;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_GET_POL_OBJ_LEN_CB, &args);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  if (args.len == 0) {
    log_error(pap_logger_id, "[%s:%d] Invalid object length.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  *pol_obj_len = args.len;

  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

pap_error_e pap_get_subjects_list_of_actions(char *subject_id, int subject_id_length, pap_action_list_t **action_list) {
  int tok_num = 0;
  jsmn_parser parser;
  jsmntok_t tokens[PAP_MAX_TOKENS];
  pap_policy_id_list_t *pol_id_list = NULL;
  pap_policy_id_list_t *prev = NULL;

  // Check input parameter
  if (subject_id == NULL || subject_id_length == 0) {
    log_error(pap_logger_id, "[%s:%d] Bad input parameters.\n", __func__, __LINE__);
    return PAP_ERROR;
  }

  pthread_mutex_lock(&pap_mutex);

  // Get all stored policy IDs
  plugin_t *plugin = NULL;
  int callback_fired = 0;
  for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
    plugin_manager_get(&g_plugin_manager, i, &plugin);
    int callback_status = -1;
    if (plugin != NULL) {
      callback_status = plugin_call(plugin, PAP_PLUGIN_GET_ALL_CB, &pol_id_list);
      callback_fired = 1;
    }
    // TODO: check status
  }

  if (callback_fired == 0) {
    log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  if (pol_id_list == NULL) {
    log_error(pap_logger_id, "[%s:%d] No policies stored.\n", __func__, __LINE__);
    pthread_mutex_unlock(&pap_mutex);
    return PAP_ERROR;
  }

  while (pol_id_list) {
    char *pol_obj = NULL;
    int action = -1;
    pap_action_list_t *action_elem = NULL;
    pap_action_list_t *action_temp = NULL;
    pap_plugin_get_args_t get_args = {0};
    pap_policy_t policy = {0};

    // Get obj. len
    plugin = NULL;
    pap_plugin_len_args_t args;
    args.policy_id = pol_id_list->policy_id;
    int callback_fired = 0;
    for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
      plugin_manager_get(&g_plugin_manager, i, &plugin);
      int callback_status = -1;
      if (plugin != NULL) {
        callback_status = plugin_call(plugin, PAP_PLUGIN_GET_POL_OBJ_LEN_CB, &args);
        callback_fired = 1;
      }
      // TODO: check status
    }

    if (callback_fired == 0) {
      log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
      pthread_mutex_unlock(&pap_mutex);
      return PAP_ERROR;
    }

    pol_obj = malloc(args.len * sizeof(char));

    get_args.policy = &policy;
    policy.policy_object.policy_object = pol_obj;
    strncpy(get_args.policy_id, pol_id_list->policy_id, PAP_POL_ID_MAX_LEN + 1);

    plugin = NULL;
    callback_fired = 0;
    for (int i = 0; i < g_plugin_manager.plugins_num; i++) {
      plugin_manager_get(&g_plugin_manager, i, &plugin);
      int callback_status = -1;
      if (plugin != NULL) {
        callback_status = plugin_call(plugin, PAP_PLUGIN_GET_CB, &get_args);
        callback_fired = 1;
      }
      // TODO: check status
    }

    if (callback_fired == 0) {
      log_error(pap_logger_id, "[%s:%d] No callback for PAP_PLUGIN_GET_CB.\n", __func__, __LINE__);
      pthread_mutex_unlock(&pap_mutex);
      return PAP_ERROR;
    }

    jsmn_init(&parser);
    tok_num = jsmn_parse(&parser, policy.policy_object.policy_object, policy.policy_object.policy_object_size, tokens,
                         sizeof(tokens) / sizeof(tokens[0]));

    for (int i = 0; i < tok_num; i++) {
      if (((subject_id_length + strlen("0x")) == (tokens[i].end - tokens[i].start)) &&
          (memcmp(policy.policy_object.policy_object + tokens[i].start + strlen("0x"), subject_id, subject_id_length) ==
           0)) {
        for (int j = 0; j < tok_num; j++) {
          if (((tokens[j].end - tokens[j].start) == strlen("action")) &&
              (memcmp(policy.policy_object.policy_object + tokens[j].start, "action", strlen("action")) == 0)) {
            action = j + 2;  // Skip 'value' token and return requested token
            break;
          }
        }
      }
    }

    if (action != -1) {
      char pol_id_str[PAP_POL_ID_MAX_LEN * 2 + 1] = {0};
      uint64_t index;

      action_elem = malloc(sizeof(pap_action_list_t));
      memset(action_elem, 0, sizeof(pap_action_list_t));

      if (hex_to_str(pol_id_list->policy_id, pol_id_str, PAP_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
        log_error(pap_logger_id, "[%s:%d] Could not convert hex value to string.\n", __func__, __LINE__);
        pthread_mutex_unlock(&pap_mutex);
        free(pol_obj);
        return PAP_ERROR;
      }

      memcpy(action_elem->action, policy.policy_object.policy_object + tokens[action].start,
             tokens[action].end - tokens[action].start);
      memcpy(action_elem->policy_id_str, pol_id_str, PAP_POL_ID_MAX_LEN * 2);
      memcpy(action_elem->is_available.cost, policy.policy_object.cost, strlen(policy.policy_object.cost));
      action_elem->next = NULL;

      if (*action_list == NULL) {
        *action_list = action_elem;
      } else {
        action_temp = *action_list;
        while (action_temp->next) {
          action_temp = action_temp->next;
        }

        action_temp->next = action_elem;
      }
    }

    free(pol_obj);

    prev = pol_id_list;
    pol_id_list = pol_id_list->next;
    free(prev);
  }

  pthread_mutex_unlock(&pap_mutex);
  return PAP_NO_ERROR;
}

void pap_user_management_action(pap_user_mng_req_e request, ...) {
  va_list valist;

  va_start(valist, request);

  switch (request) {
    case PAP_USERMNG_GET_ALL_USR: {
      char *response;
      response = va_arg(valist, char *);
      user_get_all(response);
      break;
    }
    case PAP_USERMNG_GET_USER: {
      char *id;
      char *response;
      id = va_arg(valist, char *);
      response = va_arg(valist, char *);
      user_get_obj(id, response);
      break;
    }
    case PAP_USERMNG_PUT_USER: {
      char *json_string;
      char *json_response;
      json_string = va_arg(valist, char *);
      json_response = va_arg(valist, char *);
      user_put_obj(json_string, json_response);
      break;
    }
    case PAP_USERMNG_GET_USER_ID: {
      char *username;
      char *json_string;
      username = va_arg(valist, char *);
      json_string = va_arg(valist, char *);
      user_get_user_id(username, json_string);
      break;
    }
    case PAP_USERMNG_CLR_ALL_USR: {
      char *response;
      response = va_arg(valist, char *);
      user_clear_all(response);
      break;
    }
    default:
      break;
  }
}
