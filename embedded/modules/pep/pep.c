/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
 * \project Decentralized Access Control
 * \file pep.c
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 * 18.02.2019. Added enforce_request_action from resolver module.
 ****************************************************************************/

#include <stdio.h>
#include "parson.h"
#include "pep.h"
#include "pdp.h"
#include "pep_internal.h"
#include "Dlog.h"
#include "authDacHelper.h"
#include "json_parser.h"
#include "policystore.h"

#include "resolver.h"

static char* datahex(char* string, int slength) {

	if(string == NULL)
		return NULL;

	if((slength % 2) != 0) // must be even
		return NULL;

	int dlength = 32;

	char* data = temp_policy_id;
	memset(data, 0, dlength);

	size_t index = 0;
	while (index < slength) {
		char c = string[index];
		int value = 0;
		if(c >= '0' && c <= '9')
			value = (c - '0');
		else if (c >= 'A' && c <= 'F')
			value = (10 + (c - 'A'));
		else if (c >= 'a' && c <= 'f')
			value = (10 + (c - 'a'));
		else {
			return NULL;
		}

		data[(index/2)] += value << (((index + 1) % 2) * 4);

		index++;
	}

	return data;
}

int pep_request_access(char *request, int *obl)
{
	pdp_decision_t ret = PDP_ERROR;
	policy_t *pol = NULL;
    int size = -1;
    char *request_policy_id_hex;
    int request_policy_id = -1;
    char action[15];
    int action_l = 15;


	request_policy_id = json_get_value(request, 0, "policy_id");
    size = get_size_of_token(request_policy_id);
    request_policy_id_hex = datahex(request + get_start_of_token(request_policy_id), size);

	pol = PolicyStore_get_policy(request_policy_id_hex, 32);
	ret = pdp_calculate_decision(pol, action);

	if (ret == PDP_GRANT)
		pol->num_of_executions++;

	ledControl(ret, action, obl);


	return ret;
}
