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
#include "pip.h"
#include "pap.h"

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
 * @param   void
 *
 * @return  void
 */
void TRANSACTION_init(void);

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

#endif //_TRANSACTION_H_