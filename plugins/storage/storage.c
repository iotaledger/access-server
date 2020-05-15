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
 * \project Policy Store
 * \file policystore.c
 * \brief
 * Implementation of policy store
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 01.10.2018. Added new functions that work without JSON paresr.
 ****************************************************************************/

#include <string.h>
#include <pthread.h>

#include "storage.h"
#include "json_parser.h"

#include "test_internal.h"
#include "resolver.h"
#include "utils_string.h"
#include "apiorig.h"
#include "time_manager.h"

#define Dlog_printf printf

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PS_POL_BUFF_LEN 2048
#define PS_REC_DATA_LEN 32
#define PS_STR_LEN 64
#define PS_BUFF_LEN 80
#define PS_ID_SIZE 32
#define PS_TOK_ARRAY_LEN 256

char policy_array_od[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"open_door\"},   {\"type\":\"action\",\"value\":\"open_door\"}],   \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_cd[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"close_door\"},  {\"type\":\"action\",\"value\":\"close_door\"}],  \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_h[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"honk\"},  {\"type\":\"action\",\"value\":\"honk\"}],  \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_aon[]	= {"{\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"public_id\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"request.subject.type\",\"value\":\"request.subject.value\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"alarm_on\"},{\"type\":\"action\",\"value\":\"alarm_on\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_aoff[] = {"{\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"public_id\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"request.subject.type\",\"value\":\"request.subject.value\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"alarm_off\"},{\"type\":\"action\",\"value\":\"alarm_off\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char temp_policy[PS_POL_BUFF_LEN]	= {0};
char received_data[PS_REC_DATA_LEN]	= {0};

static node_t head = {NULL, NULL};

pthread_mutex_t lock_ps;

const int MAX_NUM_OF_POLICIES = 10;

void PolicyStore_free_policy(policy_t *pol)
{
	free(pol->policy_cost);
	free(pol);
}


void PolicyStore_free_action_list_item(list_t *item)
{
	free(item);
}

int PolicyStore_init()
{
    PolicyStore_put_string(policy_array_cd, lock_Minh, "0", 1);
    PolicyStore_put_string(policy_array_od, unlock_Minh, "0", 1);
    PolicyStore_put_string(policy_array_h, honk_Minh, "0", 1);
    PolicyStore_put_string(policy_array_aon, alarm_on_Minh, "0", 1);
    PolicyStore_put_string(policy_array_aoff, alarm_off_Minh, "0", 1);

    return 0;
}

int datahex2(const char* string)
{

	if(string == NULL)
	{
		return -1;
	}

	size_t slength = PS_STR_LEN;
	size_t dlength = slength / 2;

	memset(received_data, 0, dlength);

	size_t index = 0;
	while (index < slength)
	{
		unsigned char c = string[index];
		int value = 0;
		if(c >= '0' && c <= '9')
		{
			value = (c - '0');
		}
		else if (c >= 'A' && c <= 'F')
		{
			value = (10 + (c - 'A'));
		}
		else if (c >= 'a' && c <= 'f')
		{
			value = (10 + (c - 'a'));
		}
		else
		{
			return -1;
		}

		received_data[(index/2)] += value << (((index + 1) % 2) * 4);

		index++;
	}

	return 0;
}

//@FIXME: Memory for policies is never freed. This must be implemented.
int PolicyStore_put_policy(char *policy_id, int policy_id_size, char *signed_policy, int signed_policy_size,
                           char *policy_id_signature, int policy_id_signature_size, char *policy_cost, short policy_cost_size)
{
	short size = PS_REC_DATA_LEN;
	short sa_size_offset = 0;
	short s_size_offset = 0;
	short pk_size_offset = 0;
	char *policy = malloc(signed_policy_size);
	memset(policy, 0, signed_policy_size);
	int ret = 0;
	int pos = 0;
	unsigned long long policy_size;
	policy_id_signature_t signature;

	if (policy_id == NULL ||
		policy == NULL ||
		policy_id_signature == NULL ||
		policy_cost == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Bad input parameter.\n\n", __FUNCTION__);
		return -1;
	}

	signature.signature_algorithm_size = atoi(&policy_id_signature[pos]);
	//check how many characters in buffer is taken for signature_algorithm_size
	if (signature.signature_algorithm_size < 10)
	{
		sa_size_offset = 1;
	}
	else if (signature.signature_algorithm_size < 100)
	{
		sa_size_offset = 2;
	}
	else
	{
		sa_size_offset = 3;
	}
	pos += sa_size_offset;
	signature.signature_algorithm = malloc(signature.signature_algorithm_size * sizeof(char));
	memset(signature.signature_algorithm, 0, signature.signature_algorithm_size * sizeof(char));
	memcpy(signature.signature_algorithm, &policy_id_signature[pos], signature.signature_algorithm_size);
	signature.signature_algorithm[signature.signature_algorithm_size] = '\0';
	pos += signature.signature_algorithm_size;

	signature.signature_size = atoi(&policy_id_signature[pos]);
	//check how many characters in buffer is taken for signature_size
	if (signature.signature_size < 10)
	{
		s_size_offset = 1;
	}
	else if (signature.signature_size < 100)
	{
		s_size_offset = 2;
	}
	else
	{
		s_size_offset = 3;
	}
	pos += s_size_offset;
	signature.signature = malloc(signature.signature_size * sizeof(char));
	memset(signature.signature, 0, signature.signature_size * sizeof(char));
	memcpy(signature.signature, &policy_id_signature[pos], signature.signature_size);
	signature.signature[signature.signature_size] = '\0';
	pos += signature.signature_size;

	signature.public_key_size = atoi(&policy_id_signature[pos]);
	//check how many characters in buffer is taken for public_key_size
	if (signature.public_key_size < 10)
	{
		pk_size_offset = 1;
	}
	else if (signature.public_key_size < 100)
	{
		pk_size_offset = 2;
	}
	else
	{
		pk_size_offset = 3;
	}
	pos += pk_size_offset;
	signature.public_key = malloc(signature.public_key_size * sizeof(char));
	memset(signature.public_key, 0, signature.public_key_size * sizeof(char));
	memcpy(signature.public_key, &policy_id_signature[pos], signature.public_key_size);
	signature.public_key[signature.public_key_size] = '\0';

	if ((signature.signature_algorithm_size + signature.signature_size + signature.public_key_size) > policy_id_signature_size)
	{
		Dlog_printf("\n\nERROR[%s]: Bad input parameter.\n\n", __FUNCTION__);
		return -1;
	}

	if (memcmp(signature.signature_algorithm, "ECDSA", signature.signature_algorithm_size) == 0)
	{
		if (crypto_sign_open(policy, &policy_size, signed_policy, signed_policy_size, signature.public_key) != 0)
		{
			//signature verification failed
			Dlog_printf("\n\nERROR[%s]: Signature did not match with public key. %s\n\n", __FUNCTION__,signature.public_key);
			free(policy);
			free(signature.signature_algorithm);
			free(signature.signature);
			free(signature.public_key);
			return -1;
		}
		else
		{
			//@TODO: Consider if policy id signature needs to be stored here. Until then, free memory used for policy id signature.
			free(signature.signature_algorithm);
			free(signature.signature);
			free(signature.public_key);
		}
	}
	else
	{
		//at this moment only ECDSA is supported
		Dlog_printf("\n\nERROR[%s]: Not supported signature algorithm.\n\n", __FUNCTION__);
		free(policy);
		return -1;
	}

	ret = datahex2(policy_id);

	char buf[PS_BUFF_LEN];
	getStringTime(buf, PS_BUFF_LEN);

	if(PolicyStore_has_string(received_data, size))
	{
		Dlog_printf("%s <Policy store>\t\tPolicy already in the policy store", buf);
		Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);
		ret = 1;
	}
	else if(pol_store_counter < MAX_NUM_OF_POLICIES)
	{
		Dlog_printf("%s <Policy store>\t\tNew policy added", buf);
		Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);

		memcpy(demo_pol_store[pol_store_counter], received_data, size);
		memcpy(pol_store[pol_store_counter], policy, policy_size);

		PolicyStore_put_string(pol_store[pol_store_counter], demo_pol_store[pol_store_counter], policy_cost, policy_cost_size);
		pol_store_counter++;
		//@TODO: This shouldn't be handled by Resolver
		//policy_update_indication();
	}
	else
	{
		Dlog_printf("%s <Policy store>\t\tLocal policy store full\n", buf);
	}

	free(policy);
	return ret;
}

void fill_node(node_t *n, char *policy_id, short policy_id_size, char *pol_val, char *policy_cost, short policy_cost_size, int is_paid, node_t *next)
{
	n->pol->policy_cost_size = policy_cost_size;
	n->pol->policy_cost = policy_cost;
	n->pol->policy_ID_size = policy_id_size;
	n->pol->policy_id = policy_id;
	n->pol->policy_c = pol_val;
	n->pol->is_paid = 0;
	n->pol->num_of_executions = 0;
	n->next = next;
}

int PolicyStore_put_string(char *pol_val, char *sig_val, char *policy_cost, int policy_cost_size)
{
	node_t *current = NULL;
	node_t *new_node;
	short size_of_id = PS_ID_SIZE;

	pthread_mutex_lock(&lock_ps);

	if(head.pol == NULL)
	{
		head.pol = malloc(sizeof(policy_t));
		if(head.pol == NULL)
		{
			Dlog_printf("Failed to allocate new memory for the policy");
			pthread_mutex_unlock(&lock_ps);
			return 1;
		}

		fill_node(&head, sig_val, size_of_id, pol_val, policy_cost, policy_cost_size, 0, NULL);
	}
	else
	{
		current = &head;

		while(current->next != NULL)
		{
			current = current->next;
		}

		new_node = malloc(sizeof(node_t));
		if(new_node == NULL)
		{
			Dlog_printf("Failed to allocate new memory for the node\n");
			pthread_mutex_unlock(&lock_ps);
			return 1;
		}

		new_node->pol = malloc(sizeof(policy_t));
		if(new_node->pol == NULL)
		{
			free(new_node);
			Dlog_printf("Failed to allocate new memory for the policy\n");
			pthread_mutex_unlock(&lock_ps);
			return 1;
		}

		fill_node(new_node, sig_val, size_of_id, pol_val, policy_cost, policy_cost_size, 0, NULL);

		current->next = new_node;
		new_node = NULL;
	}

	pthread_mutex_unlock(&lock_ps);
	return 0;
}

int PolicyStore_get_string(char **pol_string, char *psID, short policyIDSize)
{
	int ret = 1;
	node_t *current = NULL;

	pthread_mutex_lock(&lock_ps);

	if(head.pol == NULL)
	{
		pthread_mutex_unlock(&lock_ps);
		return ret;
	}
	else
	{
		if(head.pol->policy_ID_size == policyIDSize)
		{
			if(memcmp(head.pol->policy_id, psID, policyIDSize) == 0)
			{
				if(head.pol->policy_c == NULL)
				{
					Dlog_printf("\nPolicy string is NULL\n");
				}
				else
				{
					*pol_string = head.pol->policy_c;

					ret = 0;
				}
			}
			current = head.next;
		}

		while(current != NULL && ret != 0)
		{
			if(current->pol->policy_ID_size == policyIDSize)
			{
				if(memcmp(current->pol->policy_id, psID, policyIDSize) == 0)
				{
					if(current->pol->policy_c == NULL)
					{
						Dlog_printf("\nPolicy string is NULL\n");
					}
					else
					{
						*pol_string = current->pol->policy_c;
						ret = 0;
					}
				}
			}
			current = current->next;
		}
	}

	pthread_mutex_unlock(&lock_ps);

	return ret;
}

int PolicyStore_has_string(char *policyID, short policyIDSize)
{
	int ret = 0;
	node_t *current = NULL;

	pthread_mutex_lock(&lock_ps);

	if(head.pol == NULL)
	{
		Dlog_printf("Policy store empty");
		pthread_mutex_unlock(&lock_ps);
		return ret;
	}
	else
	{
		if(head.pol->policy_ID_size == policyIDSize)
		{
			if(memcmp(head.pol->policy_id, policyID, policyIDSize) == 0)
			{
				ret = 1;
			}
		}

		current = head.next;

		while(current != NULL && ret != 1)
		{
			if(current->pol->policy_ID_size == policyIDSize)
			{
				if(memcmp(current->pol->policy_id, policyID, policyIDSize) == 0)
				{
					ret = 1;
				}
			}
			current = current->next;
		}
	}

	pthread_mutex_unlock(&lock_ps);

	return ret;
}

int PolicyStore_del_policy(char *policyID, int policyIDSize)
{
   int ret = 1;

   node_t *current = &head;
   node_t *previous = NULL;

   pthread_mutex_lock(&lock_ps);

   // Check if empty
   if(head.next == NULL && head.pol == NULL)
   {
	   pthread_mutex_unlock(&lock_ps);
       return ret;
   }

   // Check first
   if((current->pol->policy_ID_size == policyIDSize) && (memcmp(current->pol->policy_id, policyID, policyIDSize) == 0))
   {
       ret = 0;
       if(head.next != NULL)
       {
    	   free(head.pol);
    	   head.pol = head.next->pol;
    	   head.next = head.next->next;
    	   head = *head.next;

    	   free(current);
       }
       else
       {
		   free(head.pol);
           head.next = NULL;
           head.pol = NULL;
       }
       current = NULL;
   }
   else
   {
       previous = current;
       current = current->next;
   }

   // Check rest
   while(current != NULL)
   {
       if((current->pol->policy_ID_size == policyIDSize) && (memcmp(current->pol->policy_id, policyID, policyIDSize) == 0))
       {
           ret = 0;
           previous->next = current->next;
           free(current->pol);
           free(current);
           break;
       }
       previous = current;
       current = current->next;
   }

   pthread_mutex_unlock(&lock_ps);

   return ret;
}

int PolicyStore_get_list_of_actions(char *subject_id, int subject_id_length, list_t **action_list)
{
	int ret = 0;

	node_t *current = &head;
	list_t *current_action = NULL;
	list_t *temp_action;

	static jsmn_parser p;
	static jsmntok_t t[PS_TOK_ARRAY_LEN];
	int r;
	int token = -1;
	int l = 0;

	pthread_mutex_lock(&lock_ps);

	while(current != NULL)
	{
		jsmn_init(&p);
		r = jsmn_parse(&p, current->pol->policy_c, strlen(current->pol->policy_c), t, sizeof(t)/sizeof(t[0]));

		for(int i = 0; i < r; i++)
		{
			if(((subject_id_length + 2) == (t[i].end - t[i].start)) && (memcmp(current->pol->policy_c + t[i].start + 2, subject_id, subject_id_length) == 0))
			{
				for(int i = 0; i < r; i++)
				{
					if(((t[i].end - t[i].start) == strlen("action")) && (memcmp(current->pol->policy_c + t[i].start, "action", strlen("action")) == 0))
					{
						token = i + 2;	// Skip 'value' token and return requested token
						break;
					}
				}

				temp_action = malloc(sizeof(list_t));
				if(temp_action == NULL)
				{
					Dlog_printf("\nERROR: failed to allocate memory for the new list entry");
					break;
				}


				temp_action->policyID = current->pol->policy_id;
				temp_action->action = current->pol->policy_c + t[token].start;
				temp_action->action_length = t[token].end - t[token].start;
				temp_action->policy_cost = current->pol->policy_cost;
				temp_action->policy_cost_size = current->pol->policy_cost_size;
				temp_action->next = NULL;

				if(current_action == NULL)
				{
					current_action = temp_action;
					*action_list = current_action;
				}
				else
				{
					current_action->next = temp_action;
					current_action = current_action->next;
				}

				ret++;
				break;
			}
		}

		current = current->next;
	}

	pthread_mutex_unlock(&lock_ps);

	char buf[PS_BUFF_LEN];
	getStringTime(buf, PS_BUFF_LEN);
	printf("\n%s <Action performed>\tSent list of actions\n", buf);

	return ret;
}

int PolicyStore_clean()
{
	node_t *current = NULL;
	node_t *next;

	pthread_mutex_lock(&lock_ps);

	if(head.pol == NULL)
	{
		Dlog_printf("\nPolicy store empty");
	}
	else
	{
		//Skip first four policies from the policy store
		//TODO: WHY????
		current = head.next->next->next->next;
		head.next->next->next->next = NULL;

		while(current != NULL)
		{
			next = current->next;

			free(current->pol);
			free(current);

			current = next;
		}
	}

	pthread_mutex_unlock(&lock_ps);

	return 0;
}

int PolicyStore_clear_list_of_actions(list_t *action_list)
{
	list_t *current = action_list;
	list_t *next = NULL;
	while(current != NULL)
	{
		next = current->next;
		PolicyStore_free_action_list_item(current);
		current = next;
	}

	action_list = NULL;

	return 0;
}

int PolicyStore_ps_aws_sync(char *AWS_list, int number_of_policies)
{
	int ret = 0;
	node_t *current = NULL;
	node_t *previous = NULL;
	int found = 0;
	int i = 0;

	pthread_mutex_lock(&lock_ps);

	//skip first three
	//TODO: WHY????
	current = head.next->next->next;
	previous = head.next->next;

	while(current != NULL)
	{
		for(i = 0; i < number_of_policies; i++)
		{
			datahex2(AWS_list + get_start_of_token(3 + i));
			if((get_size_of_token(3 + i) == PS_STR_LEN) &&
			   (memcmp(received_data, current->pol->policy_id, get_size_of_token(3 + i)) == 0))
			{
				found = 1;
				Dlog_printf("\npol (%.*s)", get_size_of_token(3 + i), AWS_list + get_start_of_token(3 + i));
				break;
			}
		}

		if(0 == found)
		{
			//delete policy
			previous->next = current->next;
			free(current->pol);
			free(current);

			Dlog_printf("\npolicy removed");
		}
		found = 0;

		current = previous->next;
	}

	pthread_mutex_unlock(&lock_ps);

	Dlog_printf("\npolicy store synchronized");

	return ret;
}

int PolicyStore_is_policy_paid(char *policy_id, int policy_id_size)
{
	int ret = 0;

	// string that represents human readable policy ID
	char *policy_id_str = NULL;

	// policy in policy store with given policy_id
	policy_t *pol = NULL;

	pol = PolicyStore_get_policy(policy_id, policy_id_size);

	if (pol != NULL)
	{
		ret = pol->is_paid;

		if ((pol->policy_cost_size == 1) && (memcmp(pol->policy_cost, "0", 1) == 0))
		{
			ret = -1; // policy is free
		}
	}

	policy_id_str = malloc(policy_id_size * 2 + 1);
	hex_to_str(policy_id, policy_id_str, policy_id_size);
	policy_id_str[policy_id_size * 2 ] = '\0';
    Dlog_printf("\nPay check - policy ID: %s\n", policy_id_str);
    if (ret == 0)
    {
		Dlog_printf("          - result:    NOT PAID\n");
	}
	else if (ret == 1)
	{
		Dlog_printf("          - result:    PAID\n");
	}
	else if (ret == -1)
	{
		Dlog_printf("          - result:    FREE\n");
		ret = 1;
	}

    free(policy_id_str);

    return ret;
}

int PolicyStore_enable_policy(char *policy_id, int policy_id_size)
{
	policy_t *pol = NULL;
	pol = PolicyStore_get_policy(policy_id, policy_id_size);


	if (pol == NULL)
	{
		return 1;
	}

	pol->is_paid = 1;
	return 0;
}

policy_t *PolicyStore_get_policy(char *policy_id, int policy_id_size)
{
	node_t *current = &head;
	policy_t *pol = NULL; // returning policy

	pthread_mutex_lock(&lock_ps);

	while (current != NULL)
	{
		if(memcmp(current->pol->policy_id, policy_id, policy_id_size) == 0)
		{
			pol = current->pol;
			break;
		}
		current = current->next;
	}

	pthread_mutex_unlock(&lock_ps);
	return pol;
}
