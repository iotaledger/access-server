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
 * \file storage.c
 * \brief
 * Implementation of policy storage interface
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 01.10.2018. Added new functions that work without JSON paresr.
 * 25.05.2020. Refactoring.
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pap.h"
#include "utils.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define STORAGE_SIGN_ALG_LEN 64
#define STORAGE_HASH_FN_LEN 64

#ifndef bool
#define bool _Bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/

typedef enum {
  STORAGE_OK,
  STORAGE_ERROR,
} storage_error_t;

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define RPI_MAX_STR_LEN 1024
#define RPI_ACCESS_ERR -1
#define RPI_POL_ID_MAX_LEN 32

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
static bool rpistorage_store_policy(char* policy_id, char* policy_object, int policy_object_size, char* policy_cost,
                                    char* signature, char* public_key, char* signature_algorithm, char* hash_function) {
  char pol_path[RPI_MAX_STR_LEN] = {0};
  char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
  FILE* f = NULL;

  // Check input parameters
  if ((policy_id == NULL) || (policy_object == NULL) || (policy_object_size == 0) || (policy_cost == NULL) ||
      (signature == NULL) || (public_key == NULL) || (signature_algorithm == NULL) || (hash_function == NULL)) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
    return FALSE;
  }

  // Write policy data to a file
  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/%s.txt", pol_id_str);
  f = fopen(pol_path, "w+");
  if (f == NULL) {
    printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
    return FALSE;
  }

  fwrite("policy id:", strlen("policy id:"), 1, f);
  fwrite(pol_id_str, strlen(pol_id_str), 1, f);
  fwrite("\npolicy object:", strlen("\npolicy object:"), 1, f);
  fwrite(policy_object, policy_object_size, 1, f);
  fwrite("\npolicy cost:", strlen("\npolicy cost:"), 1, f);
  fwrite(policy_cost, strlen(policy_cost), 1, f);
  fwrite("\npolicy id signature:", strlen("\npolicy id signature:"), 1, f);
  fwrite(signature, strlen(signature), 1, f);
  fwrite("\npolicy id signature public key:", strlen("\npolicy id signature public key:"), 1, f);
  fwrite(public_key, strlen(public_key), 1, f);
  fwrite("\npolicy id signature sign. algorithm:", strlen("\npolicy id signature sign. algorithm:"), 1, f);
  fwrite(signature_algorithm, strlen(signature_algorithm), 1, f);
  fwrite("\nhash function:", strlen("\nhash function:"), 1, f);
  fwrite(hash_function, strlen(hash_function), 1, f);

  fclose(f);

  // Store policy ID in stored policies file
  memset(pol_path, 0, RPI_MAX_STR_LEN * sizeof(char));
  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/stored_policies.txt");

  f = fopen(pol_path, "a");
  if (f == NULL) {
    printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
    return FALSE;
  }

  fwrite(pol_id_str, strlen(pol_id_str), 1, f);
  fwrite("|", strlen("|"), 1, f);

  fclose(f);

  return TRUE;
}

static bool rpistorage_acquire_policy(char* policy_id, char* policy_object, int* policy_object_size, char* policy_cost,
                                      char* signature, char* public_key, char* signature_algorithm,
                                      char* hash_function) {
  char pol_path[RPI_MAX_STR_LEN] = {0};
  char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
  char* buffer;
  char* substr;
  int buff_len;
  int substr_len;
  FILE* f;

  // Check input parameters
  if ((policy_id == NULL) || (policy_object == NULL) || (policy_object_size == NULL) || (policy_cost == NULL) ||
      (signature == NULL) || (public_key == NULL) || (signature_algorithm == NULL) || (hash_function == NULL)) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
    return FALSE;
  }

  // Open file
  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/%s.txt", pol_id_str);
  f = fopen(pol_path, "r");
  if (f == NULL) {
    printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
    return FALSE;
  }

  // Get file size
  fseek(f, 0L, SEEK_END);
  buff_len = ftell(f);
  fseek(f, 0L, SEEK_SET);

  // Write policy to a buffer
  buffer = malloc(buff_len * sizeof(char));

  fread(buffer, buff_len * sizeof(char), 1, f);
  fclose(f);

  // Set the arguments
  substr = strstr(buffer, "policy object:");
  substr_len = strstr(buffer, "\npolicy cost:") - (substr + strlen("policy object:"));
  memcpy(policy_object, substr + strlen("policy object:"), substr_len);

  *policy_object_size = substr_len;

  substr = strstr(buffer, "policy cost:");
  substr_len = strstr(buffer, "\npolicy id signature:") - (substr + strlen("policy cost:"));
  memcpy(policy_cost, substr + strlen("policy cost:"), substr_len);

  substr = strstr(buffer, "policy id signature:");
  substr_len = strstr(buffer, "\npolicy id signature public key:") - (substr + strlen("policy id signature:"));
  memcpy(signature, substr + strlen("policy id signature:"), substr_len);

  substr = strstr(buffer, "policy id signature public key:");
  substr_len =
      strstr(buffer, "\npolicy id signature sign. algorithm:") - (substr + strlen("policy id signature public key:"));
  memcpy(public_key, substr + strlen("policy id signature public key:"), substr_len);

  substr = strstr(buffer, "policy id signature sign. algorithm:");
  substr_len = strstr(buffer, "\nhash function:") - (substr + strlen("policy id signature sign. algorithm:"));
  memcpy(signature_algorithm, substr + strlen("policy id signature sign. algorithm:"), substr_len);

  substr = strstr(buffer, "hash function:");
  substr_len = &buffer[buff_len] - (substr + strlen("hash function:"));
  memcpy(hash_function, substr + strlen("hash function:"), substr_len);

  free(buffer);

  return TRUE;
}

static bool rpistorage_check_if_stored_policy(char* policy_id) {
  char pol_path[RPI_MAX_STR_LEN] = {0};
  char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};

  // Check input parameters
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
    return FALSE;
  }

  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/%s.txt", pol_id_str);

  // Check file existance
  if (access(pol_path, F_OK) != RPI_ACCESS_ERR) {
    // File exists
    return TRUE;
  } else {
    // File doesn't exist
    return FALSE;
  }
}

static bool rpistorage_flush_policy(char* policy_id) {
  char pol_path[RPI_MAX_STR_LEN] = {0};
  char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
  char* stored_pol_buff = NULL;
  char* stored_pol_buff_new = NULL;
  int stored_pol_buff_len = 0;
  FILE* f;

  // Check input parameters
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
    return FALSE;
  }

  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/%s.txt", pol_id_str);

  if (remove(pol_path) == 0) {
    // Remove policy ID from stored policies file
    memset(pol_path, 0, RPI_MAX_STR_LEN * sizeof(char));
    sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/stored_policies.txt");
    f = fopen(pol_path, "r");
    if (f == NULL) {
      printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
      return FALSE;
    }

    fseek(f, 0L, SEEK_END);
    stored_pol_buff_len = ftell(f);
    fseek(f, 0L, SEEK_SET);

    stored_pol_buff = malloc(stored_pol_buff_len * sizeof(char));
    stored_pol_buff_new = malloc((stored_pol_buff_len - (strlen(pol_id_str) + 1)) * sizeof(char));  // Add +1 for "|"

    fread(stored_pol_buff, stored_pol_buff_len, 1, f);

    fclose(f);

    memcpy(stored_pol_buff_new, stored_pol_buff, (strstr(stored_pol_buff, pol_id_str) - stored_pol_buff));
    memcpy(
        &stored_pol_buff_new[strstr(stored_pol_buff, pol_id_str) - stored_pol_buff],
        (strstr(stored_pol_buff, pol_id_str) + (strlen(pol_id_str) + 1)),  // Add +1 for "|"
        (&stored_pol_buff[stored_pol_buff_len] - ((strstr(stored_pol_buff, pol_id_str) + (strlen(pol_id_str) + 1)))));

    f = fopen(pol_path, "w");
    if (f == NULL) {
      printf("\nERROR[%s]: Invalid path to stored_policies file.\n", __FUNCTION__);
      return FALSE;
    }

    fwrite(stored_pol_buff_new, stored_pol_buff_len - (strlen(pol_id_str) + 1), 1, f);  // Add +1 for "|"

    fclose(f);

    free(stored_pol_buff);
    free(stored_pol_buff_new);

    // Success
    return TRUE;
  } else {
    // Fail
    return FALSE;
  }
}

static int rpistorage_get_pol_obj_len(char* policy_id) {
  char pol_path[RPI_MAX_STR_LEN] = {0};
  char pol_id_str[RPI_POL_ID_MAX_LEN * 2 + 1] = {0};
  char* buffer;
  int buff_len;
  int ret = 0;
  FILE* f;

  // Check input parameters
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return ret;
  }

  if (hex_to_str(policy_id, pol_id_str, RPI_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nERROR[%s]: Could not convert hex value to string.\n", __FUNCTION__);
    return ret;
  }

  // Open file
  sprintf(pol_path, "../plugins/pap/platforms/r_pi/policies/%s.txt", pol_id_str);
  f = fopen(pol_path, "r");
  if (f == NULL) {
    printf("\nERROR[%s]: Invalid path to file.\n", __FUNCTION__);
    return ret;
  }

  // Get file size
  fseek(f, 0L, SEEK_END);
  buff_len = ftell(f);
  fseek(f, 0L, SEEK_SET);

  // Write policy to a buffer
  buffer = malloc(buff_len * sizeof(char));

  fread(buffer, buff_len * sizeof(char), 1, f);
  fclose(f);

  ret = &buffer[buff_len] - (strstr(buffer, "policy object:") + strlen("policy object:"));

  return ret;
}

static bool store_policy(char* policy_id, pap_policy_object_t policy_object,
                         pap_policy_id_signature_t policy_id_signature, pap_hash_functions_e hash_fn) {
  char sign_algorithm[STORAGE_SIGN_ALG_LEN] = {0};
  char hash_function[STORAGE_HASH_FN_LEN] = {0};

  // Check input parameter
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // OPTIONAL: Policy ID signature can be validated here as additional level of security

  // Set hash functions
  if (hash_fn == PAP_SHA_256) {
    memcpy(hash_function, "sha-256", strlen("sha-256"));
  } else {
    printf("\nERROR[%s]: Unsupported hash function.\n", __FUNCTION__);
    return FALSE;
  }

  // Set policy ID signature algorithm
  if (policy_id_signature.signature_algorithm == PAP_ECDSA) {
    memcpy(sign_algorithm, "ECDSA", strlen("ECDSA"));
  } else {
    printf("\nERROR[%s]: Unsupported signature algorithm.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for storing policy on used platform
  return rpistorage_store_policy(policy_id, policy_object.policy_object, policy_object.policy_object_size,
                                 policy_object.cost, policy_id_signature.signature, policy_id_signature.public_key,
                                 sign_algorithm, hash_function);
}

static bool acquire_policy(char* policy_id, pap_policy_object_t* policy_object,
                           pap_policy_id_signature_t* policy_id_signature, pap_hash_functions_e* hash_fn) {
  char sign_algorithm[STORAGE_SIGN_ALG_LEN] = {0};
  char hash_function[STORAGE_HASH_FN_LEN] = {0};

  // Check input parameter
  if ((policy_id == NULL) || (policy_object == NULL) || (policy_id_signature == NULL) || (hash_fn == NULL)) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for storing policy on used platform
  if (rpistorage_acquire_policy(policy_id, policy_object->policy_object, &(policy_object->policy_object_size),
                                policy_object->cost, policy_id_signature->signature, policy_id_signature->public_key,
                                sign_algorithm, hash_function) == FALSE) {
    printf("\nERROR[%s]: Could not acquire policy from R-Pi.\n", __FUNCTION__);
    return FALSE;
  }

  if (memcmp(sign_algorithm, "ECDSA", strlen("ECDSA")) == 0) {
    policy_id_signature->signature_algorithm = PAP_ECDSA;
  } else {
    printf("\nERROR[%s]: Unsupported signature algorithm.\n", __FUNCTION__);
    return FALSE;
  }

  if (memcmp(hash_function, "sha-256", strlen("sha-256")) == 0) {
    *hash_fn = PAP_SHA_256;
  } else {
    printf("\nERROR[%s]: Unsupported hash function.\n", __FUNCTION__);
    return FALSE;
  }

  return TRUE;
}

bool check_if_stored_policy(char* policy_id) {
  // Check input parameter
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for checking if policy is stored on used platform
  return rpistorage_check_if_stored_policy(policy_id);
}

static bool flush_policy(char* policy_id) {
  // Check input parameter
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for checking if policy is stored on used platform
  return rpistorage_flush_policy(policy_id);
}

static bool acquire_pol_obj_len(char* policy_id, int* pol_obj_len) {
  // Check input parameter
  if (policy_id == NULL || pol_obj_len == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for geting if policy object length
  *pol_obj_len = rpistorage_get_pol_obj_len(policy_id);

  return TRUE;
}

// List must be freed bu the user
static bool acquire_all_policies(pap_policy_id_list_t** pol_list_head) {
  // Call function for acquring all policies
  char pol_id_hex[PAP_POL_ID_MAX_LEN] = {0};
  char* pol_id_buff = NULL;
  char* tok = NULL;
  int pol_id_buff_len = 0;
  pap_policy_id_list_t* temp = NULL;
  FILE* f;

  f = fopen("../../plugins/storage/platforms/r_pi/policies/stored_policies.txt", "r");
  if (f == NULL) {
    printf("\nERROR[%s]: No stored policies info file.\n", __FUNCTION__);
    return FALSE;
  }

  fseek(f, 0L, SEEK_END);
  pol_id_buff_len = ftell(f);
  fseek(f, 0L, SEEK_SET);

  pol_id_buff = malloc(pol_id_buff_len * sizeof(char));

  fread(pol_id_buff, pol_id_buff_len, 1, f);

  fclose(f);

  tok = strtok(pol_id_buff, "|");
  while (tok) {
    pap_policy_id_list_t* elem = malloc(sizeof(pap_policy_id_list_t));
    memset(elem, 0, sizeof(pap_policy_id_list_t));
    memset(pol_id_hex, 0, PAP_POL_ID_MAX_LEN);

    if (str_to_hex(tok, pol_id_hex, PAP_POL_ID_MAX_LEN * 2) != UTILS_STRING_SUCCESS) {
      printf("\nERROR[%s]: Could not convert string to hex value.\n", __FUNCTION__);
      return FALSE;
    }

    memcpy(elem->policy_id, pol_id_hex, PAP_POL_ID_MAX_LEN);

    if (*pol_list_head == NULL) {
      *pol_list_head = elem;
    } else {
      temp = *pol_list_head;
      while (temp->next != NULL) {
        temp = temp->next;
      }

      temp->next = elem;
    }

    tok = strtok(NULL, "|");
  }

  free(pol_id_buff);

  return TRUE;
}

static int destroy_cb(plugin_t* plugin, void* data) {
  free(plugin->callbacks);
  return 0;
}

static int put_cb(plugin_t* plugin, void* data) {
  pap_policy_t* policy = (pap_policy_t*)data;
  store_policy(policy->policy_id, policy->policy_object, policy->policy_id_signature, policy->hash_function);
  return 0;
}

static int get_cb(plugin_t* plugin, void* data) {
  papplugin_get_args_t* args = (papplugin_get_args_t*)data;
  acquire_policy(args->policy_id, &args->policy->policy_object, &args->policy->policy_id_signature,
                 &args->policy->hash_function);
  strncpy(args->policy->policy_id, args->policy_id, PAP_POL_ID_MAX_LEN);
  args->policy->policy_id[PAP_POL_ID_MAX_LEN + 1] = 0;
  return 0;
}

static int has_cb(plugin_t* plugin, void* data) {
  papplugin_has_args_t* args = (papplugin_has_args_t*)data;
  args->does_have = check_if_stored_policy(args->policy_id);
  return 0;
}

static int del_cb(plugin_t* plugin, void* data) {
  char* policy_id = (char*)data;

  flush_policy(policy_id);
  return 0;
}

static int get_len_cb(plugin_t* plugin, void* data) {
  papplugin_len_args_t* args = (papplugin_len_args_t*)data;
  acquire_pol_obj_len(args->policy_id, &args->len);
  return 0;
}

static int get_all_cb(plugin_t* plugin, void* data) {
  pap_policy_id_list_t* id_list = (pap_policy_id_list_t*)data;
  acquire_all_policies(&id_list);
  return 0;
}

int pappluginrpi_initializer(plugin_t* plugin, void* data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PAPPLUGIN_CALLBACK_COUNT);
  plugin->plugin_specific_data = NULL;
  plugin->callbacks_num = PAPPLUGIN_CALLBACK_COUNT;
  plugin->callbacks[PAPPLUGIN_PUT_CB] = put_cb;
  plugin->callbacks[PAPPLUGIN_GET_CB] = get_cb;
  plugin->callbacks[PAPPLUGIN_HAS_CB] = has_cb;
  plugin->callbacks[PAPPLUGIN_DEL_CB] = del_cb;
  plugin->callbacks[PAPPLUGIN_GET_POL_OBJ_LEN_CB] = get_len_cb;
  plugin->callbacks[PAPPLUGIN_GET_ALL_CB] = get_all_cb;
  return 0;
}
