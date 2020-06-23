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
 * \file pap_plugin.c
 * \brief
 * Implementation of policy storage interface
 *
 * @Author Dejan Nedic, Strahinja Golic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 01.10.2018. Added new functions that work without JSON paresr.
 * 25.05.2020. Refactoring.
 * 24.06.2020. Refactoring
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pap_plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pap.h"
#include "utils.h"
#ifdef USE_RPI
#include "rpi_storage.h"
#endif

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define STORAGE_SIGN_ALG_LEN 64
#define STORAGE_HASH_FN_LEN 64

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
bool store_policy(char* policy_id, pap_policy_object_t policy_object, pap_policy_id_signature_t policy_id_signature,
                  pap_hash_functions_e hash_fn) {
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
#ifdef USE_RPI
  return rpistorage_store_policy(policy_id, policy_object.policy_object, policy_object.policy_object_size,
                                 policy_object.cost, policy_id_signature.signature, policy_id_signature.public_key,
                                 sign_algorithm, hash_function);
#else
  // TODO: Support for other platforms will be added here
#endif
}

bool acquire_policy(char* policy_id, pap_policy_object_t* policy_object, pap_policy_id_signature_t* policy_id_signature,
                    pap_hash_functions_e* hash_fn) {
  char sign_algorithm[STORAGE_SIGN_ALG_LEN] = {0};
  char hash_function[STORAGE_HASH_FN_LEN] = {0};

  // Check input parameter
  if ((policy_id == NULL) || (policy_object == NULL) || (policy_id_signature == NULL) || (hash_fn == NULL)) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for storing policy on used platform
#ifdef USE_RPI
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
#else
  // TODO: Support for other platforms will be added here
#endif

  return TRUE;
}

bool check_if_stored_policy(char* policy_id) {
  // Check input parameter
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for checking if policy is stored on used platform
#ifdef USE_RPI
  return rpistorage_check_if_stored_policy(policy_id);
#else
  // TODO: Support for other platforms will be added here
#endif
}

bool flush_policy(char* policy_id) {
  // Check input parameter
  if (policy_id == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for checking if policy is stored on used platform
#ifdef USE_RPI
  return rpistorage_flush_policy(policy_id);
#else
  // TODO: Support for other platforms will be added here
#endif
}

bool acquire_pol_obj_len(char* policy_id, int* pol_obj_len) {
  // Check input parameter
  if (policy_id == NULL || pol_obj_len == NULL) {
    printf("\nERROR[%s]: Bad input parameter.\n", __FUNCTION__);
    return FALSE;
  }

  // Call function for geting if policy object length
#ifdef USE_RPI
  *pol_obj_len = rpistorage_get_pol_obj_len(policy_id);
#else
  // TODO: Support for other platforms will be added here
#endif

  return TRUE;
}

// List must be freed bu the user
bool acquire_all_policies(pap_policy_id_list_t** pol_list_head) {
  // Call function for acquring all policies
#ifdef USE_RPI
  char pol_id_hex[PAP_POL_ID_MAX_LEN] = {0};
  char* pol_id_buff = NULL;
  char* tok = NULL;
  int pol_id_buff_len = 0;
  pap_policy_id_list_t* temp = NULL;
  FILE* f;

  f = fopen(rpistorage_get_stored_pol_info_file(), "r");
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

    memcpy(elem->policy_ID, pol_id_hex, PAP_POL_ID_MAX_LEN);

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
#else
  // TODO: Support for other platforms will be added here
#endif

  return TRUE;
}

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
pap_plugin_error_t pap_plugin_init(void) {
  // Register PAP callback
  if (pap_register_callbacks(store_policy, acquire_policy, check_if_stored_policy, flush_policy, acquire_pol_obj_len,
                             acquire_all_policies) == PAP_ERROR) {
    printf("\nERROR[%s]: Error registering PAP callbacks.\n", __FUNCTION__);
    return PAP_PLUGIN_ERROR;
  }

  return PAP_PLUGIN_OK;
}

pap_plugin_error_t pap_plugin_term(void) {
  // Unregister PAP callback
  if (pap_unregister_callbacks() == PAP_ERROR) {
    printf("\nERROR[%s]: Error unregistering PAP callbacks.\n", __FUNCTION__);
    return PAP_PLUGIN_ERROR;
  }

  return PAP_PLUGIN_OK;
}
