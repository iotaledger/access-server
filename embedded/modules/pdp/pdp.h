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
 * \project Decentralized Access Control
 * \file pdp.h
 * \brief
 * Implementation of Policy Decision Point
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 04.09.2018. Initial version.
 ****************************************************************************/

#include "utility.h"
#include "policystore.h"

#define GAP      (0)
#define GRANT    (1)
#define DENY     (2)
#define CONFLICT (3)
#define FALSE    (0)
#define TRUE     (1)
#define ERROR    (666)

#define OBLIGATION_LEN (15)

typedef enum operation {
   AND, OR, NOT, EQ, LEQ, GEQ, UNDEFINED, LT, GT, IF
} operation_t;

typedef enum type {
    STRING, NUMBER, TIME, BOOLEAN, UNKNOWN
} type_t;

typedef enum pdp_decision {
	PDP_ERROR = -1,
	PDP_GAP = 0,
	PDP_GRANT = 1,
	PDP_DENY = 2,
	PDP_CONFLICT = 3
} pdp_decision_t;

typedef struct attribute_value {
    type_t type;
    void *value;
    int size;
} attribute_value_t;

typedef struct action {
	unsigned long start_time;
	unsigned long stop_time;
	char* value;
} action_t;

/**
 * @fn      pdp_decision pdp_calculate_decision(policy_t *pol)
 *
 * @brief   Function that computes decision for PEP
 *
 * @param   pol	policy
 *
 * @return  pdp_decision decision made
 */
//TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
pdp_decision_t pdp_calculate_decision(policy_t *pol, char *obligation, action_t *action);
