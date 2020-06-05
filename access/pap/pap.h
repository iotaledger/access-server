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

typedef struct PAP_policy_object
{
	int policy_object_size;
	char *policy_object;
} PAP_policy_object_t;

typedef struct policy
{
	char policy_ID[PAP_POL_ID_MAX_LEN + 1]; //Consider null character
	PAP_policy_object_t policy_object;
	PAP_policy_id_signature_t policy_id_signature;
	PAP_hash_functions_e hash_function;
} PAP_policy_t;

/****************************************************************************
 * CALLBACKS
 ****************************************************************************/
typedef bool (*put_fn)(char* policy_id, PAP_policy_object_t policy_object, PAP_policy_id_signature_t policy_id_signature, PAP_hash_functions_e hash_fn);
typedef bool (*get_fn)(char* policy_id, PAP_policy_object_t *policy_object, PAP_policy_id_signature_t *policy_id_signature, PAP_hash_functions_e *hash_fn);
typedef bool (*has_fn)(char* policy_id);
typedef bool (*del_fn)(char* policy_id);
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
 *
 * @return  PAP_error_e error status
 */
PAP_error_e PAP_register_callbacks(put_fn put, get_fn get, has_fn has, del_fn del);

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
