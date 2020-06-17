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
 * \file pdp.h
 * \brief
 * Implementation of Policy Decision Point
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 04.09.2018. Initial version.
 * 05.05.2020. Refactoring
 ****************************************************************************/
#ifndef _PDP_H_
#define _PDP_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pap.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif
#define PDP_ERROR_RET (666)

#define PDP_OBLIGATION_LEN (15)

/* If any hash function, which provides hashes longer than 256 bits, is to be used,
this will have to be adjusted accordingly. */
#define PDP_POL_ID_MAX_LEN 32

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum { AND, OR, NOT, EQ, LEQ, GEQ, UNDEFINED, LT, GT, IF } pdp_operation_e;

typedef enum { STRING, NUMBER, TIME, BOOLEAN, UNKNOWN } pdp_type_e;

typedef enum { PDP_ERROR = -1, PDP_GAP = 0, PDP_GRANT = 1, PDP_DENY = 2, PDP_CONFLICT = 3 } pdp_decision_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct attribute_value {
  pdp_type_e type;
  void* value;
  int size;
} pdp_attribute_value_t;

typedef struct action {
  char pol_id_str[2 * PDP_POL_ID_MAX_LEN + 1];
  unsigned long start_time;
  unsigned long stop_time;
  unsigned long balance;
  char* wallet_address;
  char* transaction_hash;
  int transaction_hash_len;
  char* value;
  pap_action_list_t* action_list;
} pdp_action_t;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      pdp_init
 *
 * @brief   Initialize module
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pdp_init(void);

/**
 * @fn      pdp_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pdp_term(void);

/**
 * @fn      pdp_calculate_decision
 *
 * @brief   Function that computes decision for PEP
 *
 * @param   request_norm - Normalized request from user
 * @param   obligation - Obligation buffer
 * @param   action - Action data
 *
 * @return  pdp_decision decision made
 */
// TODO: obligations should be linked list of the elements of the 'obligation_s' structure type
pdp_decision_e pdp_calculate_decision(char* request_norm, char* obligation, pdp_action_t* action);

#endif  //_PDP_H_
