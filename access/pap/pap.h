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
#include <stdint.h>
#include <stdarg.h>
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
typedef enum
{
	PAP_NO_ERROR,
	PAP_ERROR
} PAP_error_e;

typedef enum
{
	//Only ECDSA is supported, for now
	PAP_ECDSA
} PAP_signature_algorithm_e;

typedef enum
{
	/* Only sha-256 is supported, for now. */
	PAP_SHA_256
} PAP_hash_functions_e;

typedef enum
{
	PAP_NOT_PAYED,
	PAP_PAYED_PENDING,
	PAP_PAYED_VERIFIED
} PAP_payment_state_e;

typedef enum
{
	PAP_USERMNG_GET_ALL_USR,
	PAP_USERMNG_GET_USER,
	PAP_USERMNG_PUT_USER,
	PAP_USERMNG_GET_USER_ID,
	PAP_USERMNG_CLR_ALL_USR
} PAP_user_mng_req_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct policy_id_signature
{
	PAP_signature_algorithm_e signature_algorithm;
	char signature[PAP_SIGNATURE_LEN];
	char public_key[PAP_PUBLIC_KEY_LEN];
} PAP_policy_id_signature_t;

typedef struct policy_value
{
	PAP_payment_state_e is_payed;
	char cost[PAP_MAX_COST_LEN];
	char wallet_address[PAP_WALLET_ADDR_LEN];
} PAP_policy_value_t;

typedef struct policy_object
{
	int policy_object_size;
	char *policy_object;
	char cost[PAP_MAX_COST_LEN];
} PAP_policy_object_t;

typedef struct policy
{
	char policy_ID[PAP_POL_ID_MAX_LEN + 1]; //Consider null character
	PAP_policy_object_t policy_object;
	PAP_policy_id_signature_t policy_id_signature;
	PAP_hash_functions_e hash_function;
} PAP_policy_t;

typedef struct policy_id_list
{
	char policy_ID[PAP_POL_ID_MAX_LEN + 1]; //Consider null character
	struct policy_id_list *next;
} PAP_policy_id_list_t;

typedef struct action_list
{
	char policy_ID_str[PAP_POL_ID_MAX_LEN * 2 + 1]; //Consider null character
	char action[PAP_MAX_STR_LEN];
	PAP_policy_value_t is_available;
	struct action_list *next;
} PAP_action_list_t;

/****************************************************************************
 * CALLBACKS
 ****************************************************************************/
typedef bool (*put_fn)(char* policy_id, PAP_policy_object_t policy_object, PAP_policy_id_signature_t policy_id_signature, PAP_hash_functions_e hash_fn);
typedef bool (*get_fn)(char* policy_id, PAP_policy_object_t *policy_object, PAP_policy_id_signature_t *policy_id_signature, PAP_hash_functions_e *hash_fn);
typedef bool (*has_fn)(char* policy_id);
typedef bool (*del_fn)(char* policy_id);
typedef bool (*get_pol_obj_len_fn)(char* policy_id, int *pol_obj_len);
typedef bool (*get_all_fn)(PAP_policy_id_list_t **pol_list_head); //List acquired here, must be freed by the caller
#if PAP_STORAGE_TEST_ACIVE
typedef void (*get_pk)(char* pk);
#endif

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      PAP_init
 *
 * @brief   Initialize module
 *
 * @param   void
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_init(void);

/**
 * @fn      PAP_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_term(void);

/**
 * @fn      PAP_register_callbacks
 *
 * @brief   Register callbacks for plugin
 *
 * @param   put - Callback for writting policy data to storage
 * @param   get - Callback for reading policy data from storage
 * @param   has - Callback for checking if the policy is in the storage
 * @param   del - Callback for deleting policy data from the storage
 * @param   get_pol_obj_len - Callback for getting stored policy's pol. object len
 * @param   get_all - Callback for getting all stored policy IDs
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_register_callbacks(put_fn put, get_fn get, has_fn has, del_fn del, get_pol_obj_len_fn get_pol_obj_len, get_all_fn get_all);

/**
 * @fn      PAP_unregister_callbacks
 *
 * @brief   Unregister callbacks for plugin
 *
 * @param   void
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_unregister_callbacks(void);

/**
 * @fn      PAP_add_policy
 *
 * @brief   Add new policy from cloud to embedded storage
 *
 * @param   signed_policy - Signed policy string buffer
 * @param   signed_policy_size - Size of the signed policy string buffer
 * @param   parsed_policy_id - Buffer to store policy ID
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_add_policy(char *signed_policy, int signed_policy_size, char *parsed_policy_id);

/**
 * @fn      PAP_get_policy
 *
 * @brief   Acquire policy data in order to make decision upon the request
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 * @param   policy - Requested policy data
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_get_policy(char *policy_id, int policy_id_len, PAP_policy_t *policy);

/**
 * @fn      PAP_has_policy
 *
 * @brief   Check if the policy is already stored
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 *
 * @return  TRUE - policy is in the storage
 *          FALSE - policy is not in the storage
 */
bool PAP_has_policy(char *policy_id, int policy_id_len);

/**
 * @fn      PAP_remove_policy
 *
 * @brief   Delete policy from embedded storage
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_remove_policy(char *policy_id, int policy_id_len);

/**
 * @fn      PAP_get_policy_obj_len
 *
 * @brief   Get length of the stored policy objected
 *
 * @param   policy_id - Policy ID as a string
 * @param   policy_id_len - Length of the policy ID string
 * @param   pol_obj_len - Length of the policy object
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_get_policy_obj_len(char *policy_id, int policy_id_len, int *pol_obj_len);

/**
 * @fn      PAP_get_subjects_list_of_actions
 *
 * @brief   Get list of actions available for specified user
 *
 * @param   subject_id - User's ID
 * @param   subject_id_length - Length of the user ID
 * @param   action_list - List of available actions (needs to be freed by the caller)
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_get_subjects_list_of_actions(char *subject_id, int subject_id_length, PAP_action_list_t **action_list);

#if PAP_STORAGE_TEST_ACIVE
/**
 * @fn      PAP_register_get_pk_cb
 *
 * @brief   Register callbacks for getting public key, if internal test is active
 *
 * @param   cb - Callback to register
 *
 * @return  void
 */
void PAP_register_get_pk_cb(get_pk cb);
#endif

/**
 * @fn      PAP_user_management_action
 *
 * @brief   Access User Management API
 *
 * @param   request - Request User Management's action
 * @param   others - Action depended
 *
 * @return  void
 */
void PAP_user_management_action(PAP_user_mng_req_e request, ...);
#endif //_PAP_H_
