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
 * \file transaction.h
 * \brief
 * Implementation of API for transactions storage
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 10.06.2020. Initial version.
 ****************************************************************************/
#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "wallet.h"

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

//Set R-Pi as used platform
#define USE_RPI 1

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum
{
	TRANS_NOT_PAYED,
	TRANS_PAYED,
	TRANS_PAYED_VERIFIED
} TRANSACTION_payment_state_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct serv_confirm
{
	confirmation_service_t *service;
	char* policy_id;
	int policy_id_len;
	bool transaction_confirmed;
} TRANSACTION_serv_confirm_t;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      TRANSACTION_init
 *
 * @brief   Initialize module
 *
 * @param   wallet_ctx - Device wallet
 *
 * @return  void
 */
void TRANSACTION_init(wallet_ctx_t* wallet_ctx);

/**
 * @fn      TRANSACTION_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  void
 */
void TRANSACTION_term(void);

/**
 * @fn      TRANSACTION_store_transaction
 *
 * @brief   Save transaction info
 *
 * @param   policy_id - Policy ID string
 * @param   policy_id_len - Length of policy ID string
 * @param   transaction_hash - Transaction hasn string
 * @param   transaction_hash_len - Transaction hasn string length
 *
 * @return  TRUE - success, FALSE - fail
 */
bool TRANSACTION_store_transaction(char* policy_id, int policy_id_len,
									char* transaction_hash, int transaction_hash_len);

#endif //_TRANSACTION_H_