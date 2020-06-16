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
 * \file rpi_storage.h
 * \brief
 * Implementation of policy storage API for Raspberry Pi 
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.05.2020. Initial version.
 ****************************************************************************/
#ifndef _RPI_STORAGE_H_
#define _RPI_STORAGE_H_

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

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      RPI_store_policy
 *
 * @brief   Save policy on R-PI file system
 * 
 * @param   policy_id - policy ID
 * @param   policy_object - policy object
 * @param   policy_object_size - policy object size
 * @param   policy_cost - policy cost
 * @param   signature - policy ID signature
 * @param   public_key - public key for policy ID signature
 * @param   signature_algorithm - policy ID signature algorithm
 * @param   hash_function - hash function
 *
 * @return  TRUE - success, FALSE - error
 */
bool RPI_store_policy(char* policy_id, char* policy_object, int policy_object_size,
                      char* policy_cost, char* signature, char* public_key,
                      char* signature_algorithm, char* hash_function);

/**
 * @fn      RPI_acquire_policy
 *
 * @brief   Acquire policy from R-PI file system
 * 
 * @param   policy_id - policy ID
 * @param   policy_object - policy object
 * @param   policy_object_size - policy object size
 * @param   policy_cost - policy cost
 * @param   signature - policy ID signature
 * @param   public_key - public key for policy ID signature
 * @param   signature_algorithm - policy ID signature algorithm
 * @param   hash_function - hash function
 *
 * @return  TRUE - success, FALSE - error
 */
bool RPI_acquire_policy(char* policy_id, char* policy_object, int *policy_object_size,
                        char* policy_cost, char* signature, char* public_key,
                        char* signature_algorithm, char* hash_function);

/**
 * @fn      RPI_check_if_stored_policy
 *
 * @brief   Check if policy is stored
 * 
 * @param   policy_id - policy ID
 *
 * @return  TRUE - stored, FALSE - not stored
 */
bool RPI_check_if_stored_policy(char* policy_id);

/**
 * @fn      RPI_flush_policy
 *
 * @brief   Delete policy from R-PI file system
 * 
 * @param   policy_id - policy ID
 *
 * @return  TRUE - success, FALSE - error
 */
bool RPI_flush_policy(char* policy_id);

/**
 * @fn      RPI_get_pol_obj_len
 *
 * @brief   Get length of stored policy's policy object
 *
 * @param   policy_id - policy ID
 *
 * @return  Length of stored policy's policy object
 */
int RPI_get_pol_obj_len(char* policy_id);

/**
 * @fn      RPI_get_stored_pol_info_file
 *
 * @brief   Acquire path to the file which keeps stored policies info
 *
 * @param   void
 *
 * @return  Path to the file which keeps stored policies info
 */
char* RPI_get_stored_pol_info_file(void);

#endif //_RPI_STORAGE_H_