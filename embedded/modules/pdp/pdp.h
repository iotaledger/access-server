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
pdp_decision_t pdp_calculate_decision(policy_t *pol, char *obligation, char *action);
