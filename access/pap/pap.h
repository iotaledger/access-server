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
 * \file pap.h
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
#ifndef _PAP_H_
#define _PAP_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdarg.h>
#include <stdint.h>
#include "plugin.h"
#include "policy_loader.h"
#include "user.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* If any hash function, which provides hashes longer than 256 bits, is to be used,
this will have to be adjusted accordingly. */
#define PAP_POL_ID_MAX_LEN 32

#define PAP_PUBLIC_KEY_LEN 32
#define PAP_PRIVATE_KEY_LEN 64
#define PAP_SIGNATURE_LEN 64

#define PAP_MAX_STR_LEN 128
#define PAP_MAX_COST_LEN 64
#define PAP_WALLET_ADDR_LEN 81

#define PAP_STORAGE_TEST_ACIVE 0

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum { PAP_NO_ERROR, PAP_ERROR } pap_error_e;

typedef enum {
  // Only ECDSA is supported, for now
  PAP_ECDSA
} pap_signature_algorithm_e;

typedef enum {
  /* Only sha-256 is supported, for now. */
  PAP_SHA_256
} pap_hash_functions_e;

typedef enum { PAP_NOT_PAYED, PAP_PAYED_PENDING, PAP_PAYED_VERIFIED } pap_payment_state_e;

typedef enum {
  PAP_USERMNG_GET_ALL_USR,
  PAP_USERMNG_GET_USER,
  PAP_USERMNG_PUT_USER,
  PAP_USERMNG_GET_USER_ID,
  PAP_USERMNG_CLR_ALL_USR
} pap_user_mng_req_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct {
  pap_signature_algorithm_e signature_algorithm;
  char signature[PAP_SIGNATURE_LEN];
  char public_key[PAP_PUBLIC_KEY_LEN];
} pap_policy_id_signature_t;

typedef struct {
  pap_payment_state_e is_payed;
  char cost[PAP_MAX_COST_LEN];
  char wallet_address[PAP_WALLET_ADDR_LEN];
} pap_policy_value_t;

typedef struct {
  int policy_object_size;
  char *policy_object;
  char cost[PAP_MAX_COST_LEN];
} pap_policy_object_t;

typedef struct {
  char policy_id[PAP_POL_ID_MAX_LEN + 1];  // Consider null character
  pap_policy_object_t policy_object;
  pap_policy_id_signature_t policy_id_signature;
  pap_hash_functions_e hash_function;
} pap_policy_t;

typedef struct policy_id_list_ {
  char policy_id[PAP_POL_ID_MAX_LEN + 1];  // Consider null character
  struct policy_id_list_ *next;
} pap_policy_id_list_t;

typedef struct pap_action_list_ {
  char policy_id_str[PAP_POL_ID_MAX_LEN * 2 + 1];  // Consider null character
  char action[PAP_MAX_STR_LEN];
  pap_policy_value_t is_available;
  struct pap_action_list_ *next;
} pap_action_list_t;

#if PAP_STORAGE_TEST_ACIVE
typedef void (*get_pk)(char *pk);
#endif

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      pap_init
 *
 * @brief   Initialize module
 *
 * @param   void
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_init(void);

/**
 * @fn      pap_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_term(void);

/**
 * @fn      pap_register_callbacks
 *
 * @brief   Register callbacks for plugin
 *
 * @param   put - Callback for writting policy data to storage
 * @param   get - Callback for reading policy data from storage
 * @param   has - Callback for checking if the policy is in the storage
 * @param   del - Callback for deleting policy data from the storage
 * @param   get_pol_obj_len - Callback for getting stored policy's object length
 * @param   get_all - Callback for getting all stored policy IDs
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_register_plugin(plugin_t *plugin);

/**
 * @fn      pap_add_policy
 *
 * @brief   Add new policy from cloud to embedded storage
 *
 * @param   signed_policy - Signed policy string buffer
 * @param   signed_policy_size - Size of the signed policy string buffer
 * @param   parsed_policy_id - Buffer to store policy ID
 * @param   owner_public_key - Owner's public key, used to verify signed policy signature
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_add_policy(char *signed_policy, int signed_policy_size, char *parsed_policy_id, char *owner_public_key);

/**
 * @fn      pap_get_policy
 *
 * @brief   Acquire policy data in order to make decision upon the request
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 * @param   policy - Requested policy data
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_get_policy(char *policy_id, int policy_id_len, pap_policy_t *policy);

/**
 * @fn      pap_has_policy
 *
 * @brief   Check if the policy is already stored
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 *
 * @return  TRUE - policy is in the storage
 *          FALSE - policy is not in the storage
 */
bool pap_has_policy(char *policy_id, int policy_id_len);

/**
 * @fn      pap_remove_policy
 *
 * @brief   Delete policy from embedded storage
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_remove_policy(char *policy_id, int policy_id_len);

/**
 * @fn      pap_get_policy_obj_len
 *
 * @brief   Get length of the stored policy objected
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 * @param   pol_obj_len - Length of the policy object
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_get_policy_obj_len(char *policy_id, int policy_id_len, int *pol_obj_len);

/**
 * @fn      pap_get_subjects_list_of_actions
 *
 * @brief   Get list of actions available for specified user
 *
 * @param   subject_id - User's ID
 * @param   subject_id_length - Length of the user ID
 * @param   action_list - List of available actions (needs to be freed by the caller)
 *
 * @return  pap_error_e error status
 */
pap_error_e pap_get_subjects_list_of_actions(char *subject_id, int subject_id_length, pap_action_list_t **action_list);

/**
 * @fn      pap_user_management_action
 *
 * @brief   Access User Management API
 *
 * @param   request - Request User Management's action
 * @param   others - Action depended
 *
 * @return  void
 */
void pap_user_management_action(pap_user_mng_req_e request, ...);
#endif  //_PAP_H_
