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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"
#include "policystore.h"
#include "json_parser.h"

#include "sha256.h"
#include "zlib.h"
#include "test_internal.h"
#include "resolver.h"
#include "utils_string.h"

#include "Dlog.h"

#include <pthread.h>
#define Dlog_printf printf

#define TRUE 1
#define FALSE 0

//char policy_array[]		= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"c73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"c73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"open_door\"},{\"type\":\"action\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_od[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"open_door\"},   {\"type\":\"action\",\"value\":\"open_door\"}],   \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_cd[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"close_door\"},  {\"type\":\"action\",\"value\":\"close_door\"}],  \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_h[]	= {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"honk\"},  {\"type\":\"action\",\"value\":\"honk\"}],  \"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_aon[]	= {"{\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"public_id\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"request.subject.type\",\"value\":\"request.subject.value\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"alarm_on\"},{\"type\":\"action\",\"value\":\"alarm_on\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};
char policy_array_aoff[] = {"{\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"public_id\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"request.subject.type\",\"value\":\"request.subject.value\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"action\",\"value\":\"alarm_off\"},{\"type\":\"action\",\"value\":\"alarm_off\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char temp_policy[2048]	= {0};
char received_data[32]	= {0};

static node_t head = {NULL, NULL};

pthread_mutex_t lock_ps;

const int MAX_NUM_OF_POLICIES = 10;

void free_policy(policy_t *pol) {
	free(pol->policy_cost);
	//free(pol->policy_id);
	//free(pol->policy_c);
	free(pol);
}

void free_action_list_item(list_t *item)
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

int datahex2(const char* string) {

	if(string == NULL)
		return -1;

	size_t slength = 64;

	size_t dlength = slength / 2;

	memset(received_data, 0, dlength);

	size_t index = 0;
	while (index < slength) {
		unsigned char c = string[index];
		int value = 0;
		if(c >= '0' && c <= '9')
			value = (c - '0');
		else if (c >= 'A' && c <= 'F')
			value = (10 + (c - 'A'));
		else if (c >= 'a' && c <= 'f')
			value = (10 + (c - 'a'));
		else {
			return -1;
		}

		received_data[(index/2)] += value << (((index + 1) % 2) * 4);

		index++;
	}

	return 0;
}

int PolicyStore_put_policy_from_aws(char *policy_id, int policy_id_size, char *policy, int policy_size, char *policy_cost, short policy_cost_size)
{
	int ret = 0;
	short size = 32;

	ret = datahex2(policy_id);

	char buf[80];
	get_time(buf);

	//Dlog_printf("%s <Policy store>\t\tNew policy added", buf);
	//Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);

	if(PolicyStore_has_string(received_data, size))
	{
		Dlog_printf("%s <Policy store>\t\tPolicy already in the policy store", buf);
		Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);
		ret = 1;
	}
	else if(pol_store_counter < MAX_NUM_OF_POLICIES)//5
	{
		Dlog_printf("%s <Policy store>\t\tNew policy added", buf);
		Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);

		memcpy(demo_pol_store[pol_store_counter], received_data, size);
		memcpy(pol_store[pol_store_counter], policy, policy_size);

		PolicyStore_put_string(pol_store[pol_store_counter], demo_pol_store[pol_store_counter], policy_cost, policy_cost_size);
		pol_store_counter++;
		policy_update_indication();

		//Dlog_printf("\n%s <Policy store> New policy added to the policy store from the AWS", buf);
		//Dlog_printf("%s <Policy store>\t\tNew policy added", buf);
		//Dlog_printf("- ID: %.*s\n", policy_id_size, policy_id);
	}
	else
	{
		Dlog_printf("%s <Policy store>\t\tLocal policy store full\n", buf);
		//Dlog_printf("\nLocal policy store full");
	}

	return ret;
}

int normalizeJSON()
{

  return 0;
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
	short size_of_id = 32;

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

int psGetString(char **pol_string, char *psID, short policyIDSize)
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
					pthread_mutex_unlock(&lock_ps);
					return ret;
				}
			}
			current = head.next;
		}

		while(current != NULL)
		{
			if(current->pol->policy_ID_size == policyIDSize)
			{
				if(memcmp(current->pol->policy_id, psID, policyIDSize) == 0)
				{
					if(head.pol->policy_c == NULL)
					{
						Dlog_printf("\nPolicy string is NULL\n");
					}
					else
					{
						*pol_string = current->pol->policy_c;

						ret = 0;
						pthread_mutex_unlock(&lock_ps);
						return ret;
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
				//TODO: move ret to the end and in conditions while ret != 1;
				ret = 1;
				pthread_mutex_unlock(&lock_ps);
				return ret;
			}
		}

		current = head.next;

		while(current != NULL)
		{
			if(current->pol->policy_ID_size == policyIDSize)
			{
				if(memcmp(current->pol->policy_id, policyID, policyIDSize) == 0)
				{
					ret = 1;
					pthread_mutex_unlock(&lock_ps);
					return ret;
				}
			}
			current = current->next;
		}
	}

	pthread_mutex_unlock(&lock_ps);

	return ret;
}

int psDel(char *policyID, int policyIDSize)
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
           //head = *head.next;

    	   //test this
    	   free(head.pol);
    	   current = head.next;
    	   head.pol = head.next->pol;
    	   head.next = head.next->next;

    	   free(current);
       }
       else
       {
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
	static jsmntok_t t[256];
	int r;

	int token = -1;

	int l = 0;
//	Dlog_printf("\nsubject_id_length (%d)", subject_id_length);
	//Dlog_printf("\nSize of token: subject_id (%.*s)", subject_id_length, subject_id);

	pthread_mutex_lock(&lock_ps);

	while(current != NULL)
	{
//		Dlog_printf("\n(%d)", l++);
		jsmn_init(&p);
		r = jsmn_parse(&p, current->pol->policy_c, strlen(current->pol->policy_c), t, sizeof(t)/sizeof(t[0]));

		for(int i = 0; i < r; i++)
		{
			if(((subject_id_length + 2) == (t[i].end - t[i].start)) && (memcmp(current->pol->policy_c + t[i].start + 2, subject_id, subject_id_length) == 0))
			{
				for(int i = 0; i < r; i++)
				{
					if(((t[i].end - t[i].start) == 6) && (memcmp(current->pol->policy_c + t[i].start, "action", 6) == 0))
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

//				Dlog_printf("\nnew action");

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

//	printf("\nFinished the list!\n");

	char buf[80];

	get_time(buf);
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
		current = head.next->next->next->next;
		head.next->next->next->next = NULL;
		/*
		  //Clears all policies from the policy store
		  free(head.pol);
	      head.pol = NULL;
	      head.next = NULL;
		 */
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
		free_action_list_item(current);
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
	current = head.next->next->next;
	previous = head.next->next;

	while(current != NULL)
	{
		for(i = 0; i < number_of_policies; i++)
		{
			datahex2(AWS_list + get_start_of_token(3 + i));
			if((get_size_of_token(3 + i) == 64) &&
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

void print_policy_id(char *policy_id, int policy_id_size)
{
	char *pol_id = calloc(policy_id_size + 1, 1);
	memcpy(pol_id, policy_id, policy_id_size);
	pol_id[policy_id_size] = '\0';

	Dlog_printf("Policy id: %s\n\n", pol_id);
	free(pol_id);
}

int PolicyStore_enable_policy(char *policy_id, int policy_id_size)
{
	policy_t *pol = NULL;
	pol = PolicyStore_get_policy(policy_id, policy_id_size);


	if (pol == NULL)
		return 1;

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
