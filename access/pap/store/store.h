/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \project Policy Store
 * \file policystore.h
 * \brief
 * Implementation of policy store
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 ****************************************************************************/

#ifndef _POLICYSOTRE_H_
#define _POLICYSOTRE_H_

#include <stdio.h>
#include <stdlib.h>
#include "parson.h"

#define HASH_L (32)

typedef struct policy {
    short policy_ID_size;
    int policy_size_uncompressed;
    int policy_size_compressed;
    short policy_cost_size;
    int is_paid;
    int num_of_executions;

	char *policy_cost;
    char *policy_id;
    char *policy_c;
} policy_t;

typedef enum {
        OK,
        ERROR,
        } error_t;

typedef struct node {
    policy_t *pol;

    struct node * next;
} node_t;

typedef struct list {
	char *policyID;
	char *action;
	int action_length;

	char *policy_cost;
	int policy_cost_size;

	struct list *next;
} list_t;

/**
 * @fn 		void PolicyStore_free_policy(policy_t *pol)
 *
 * @brief 	Function that frees memory allocated for policy_t structure
 * 			and its fields.
 *
 * @param	pol Pointer to policy structure
 */
void PolicyStore_free_policy(policy_t *pol);

/**
 * @fn 		free_ps_list_item(list_t *item)
 *
 * @brief	Function that frees memoty allocated for structure
 * 			representing action item and its fields.
 * 			Does not frees memory for pointer representing "next"
 * 			element.
 *
 * @param	item Pointer to action
 */
void PolicyStore_free_action_list_item(list_t *item);

/**
 * @fn      int PolicyStore_normalizeJSON()
 *
 * @brief   Function that takes policy and normalizes it
 *
 * @return  0.
 */
int PolicyStore_normalizeJSON();

/**
 * @fn      int PolicyStore_init( )
 *
 * @brief   Function that initiates policy store, puts three policies in the policy store
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int PolicyStore_init( );

/**
 * @fn      int PolicyStore_put_policy_from_aws(char *policy_id, int policy_id_size, char *policy, int policy_size)
 *
 * @brief   Function that receives policy from AWS and puts new policy to the policy store
 *
 * @param   policy_id       	policy ID
 * @param	policy_id_size		policy id size
 * @param	policy				policy
 * @param   policy_size     	policy size
 * @param	policy_cost			policy cost
 * @param	policy_cost_size	policy cost size
 *
 * @return  0 if it succeeds, 1 if the policy is already located in the policy store.
 */
int PolicyStore_put_policy_from_aws(char *policy_id, int policy_id_size, char *policy, int policy_size, char *policy_cost, short policy_cost_size);

/**
 * @fn      int PolicyStore_put_string(char *pol_val, char *sig_val)
 *
 * @brief   Function that checks policy store for policy with id equal to policyID
 *
 * @param   pol_val        Policy ID
 * @param   sig_val        Policy ID size
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int PolicyStore_put_string(char *pol_val, char *sig_val, char *policy_cost, int policy_cost_size); // TODO add parameters to documentation

/**
 * @fn      int PolicyStore_get_string(char **pol_string, char *psID, short policyIDSize)
 *
 * @brief   Function that searches policy store for policy with the id equal to policyID and returns policy if it exists
 *
 * @param   pol_string      Pointer to memory where requested policy is stored
 * @param   psID            Policy ID
 * @param   policyIDSize    Policy ID size
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int PolicyStore_get_string(char **pol_string, char *psID, short policyIDSize);

/**
 * @fn      int PolicyStore_has_string(char *policyID, short policyIDSize)
 *
 * @brief   Function that checks policy store for policy with id equal to policyID
 *
 * @param   policyID        Policy ID
 * @param   policyIDSize    Policy ID size
 *
 * @return  1 if it succeeds, 0 if it fails.
 */
int PolicyStore_has_string(char *policyID, short policyIDSize);

/**
 * @fn      int PolicyStore_del_policy(char *policyID, int policyIDSize)
 *
 * @brief   Function that deletes policy from the policy store if the policy exists
 *
 * @param   policyID        ID of the requested policy
 * @param   policyIDSize    Size of policyID
 *
 * @return  0 if it succeeds, 1 if it policy doesn't exist.
 */
int PolicyStore_del_policy(char *policyID, int policyIDSize);

/**
 * @fn      int PolicyStore_get_list_of_actions(char *subject_id, int subject_id_length, list_t **action_list)
 *
 * @brief   Function that checks policy store for policy that contains subject id
 *
 * @param   subject_id        	subject id
 * @param   subject_id_length	subject id length
 * @param	action_list			list of requested action
 *
 * @return  number of policies that contain subject id.
 */
int PolicyStore_get_list_of_actions(char *subject_id, int subject_id_length, list_t **action_list);

/**
 * @fn      int PolicyStore_clean()
 *
 * @brief   Function that deletes all the policies from the policy store, except first three
 *
 * @return  0.
 */
int PolicyStore_clean();

/**
 * @fn      int PolicyStore_clear_list_of_actions(list_t **action_list)
 *
 * @brief   Function that frees allocated memory for the list of actions
 *
 * @param	action_list list of actions
 *
 * @return  0.
 */
int PolicyStore_clear_list_of_actions(list_t *action_list);

/**
 * @fn      int PolicyStore_ps_aws_sync(char *AWS_list, int number_of_policies)
 *
 * @brief   Function that synchronizes local policy store with AWS version.
 * 			Deletes all policies it can not find on AWS.
 *
 * @param	AWS_list 				AWS list of policies
 * @param	number_od_policies		number of policies in AWS policy store
 *
 * @return  0.
 */
int PolicyStore_ps_aws_sync(char *AWS_list, int number_of_policies);

 /**
  * @fn 	int PolicyStore_is_policy_paid(char *policy_id)
  *
  * @brief 	Function that checks if policy is paid or not
  *
  * @param 	policy_id 		policy ID
  * @param 	policy_id_size 	policy ID size
  *
  * @return 0 if policy is NOT paid, 1 if policy IS paid
  */
int PolicyStore_is_policy_paid(char *policy_id, int policy_id_size);

/**
 * @fn		int PolicyStore_enable_policy(char *policy_id, int policy_id_size)
 *
 * @brief	Function that enables policy. Policy is disabled if it
 * 			is not paid.
 *
 * @param	policy_id		policy ID
 * @param 	policy_id_size	policy ID size
 *
 * @return 	0 if policy is successfully enabled, 1 otherwise
 */
int PolicyStore_enable_policy(char *policy_id, int policy_id_size);

policy_t *PolicyStore_get_policy(char *policy_id, int policy_id_size);

#endif // _POLICYSOTRE_H_
