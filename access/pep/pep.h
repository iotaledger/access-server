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
 * \file pep.h
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 * 11.05.2020. Refactoring
 ****************************************************************************/
#ifndef PEP_H
#define PEP_H

/****************************************************************************
 * INCLUDES
 * **************************************************************************/
#include "wallet.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/****************************************************************************
 * CALLBACKS
 ****************************************************************************/
typedef bool (*resolver_fn)(char* obligation, void* action);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/

/**
 * @fn      PEP_init
 *
 * @brief   Initialize module
 *
 * @param   wallet_ctx - Context of the device's wallet
 *
 * @return  TRUE on success, FALSE on failure
 */
bool PEP_init(wallet_ctx_t* wallet_ctx);

/**
 * @fn      PEP_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool PEP_term(void);

/**
 * @fn      PEP_register_callback
 *
 * @brief   Register resolver callback
 *
 * @param   resolver - Resolver callback
 *
 * @return  TRUE on success, FALSE on failure
 */
bool PEP_register_callback(resolver_fn resolver);

/**
 * @fn      PEP_unregister_callback
 *
 * @brief   Unregister resolver callback
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool PEP_unregister_callback(void);

/**
 * @fn      int pep_request_access(JSON_Value *request)
 *
 * @brief   Function that computes decision for PEP
 *
 * @param   request         request JSON containing request uri and request object
 *
 * @return  DENY (0), GRANT (1)
 */
bool PEP_request_access(char *request);

#endif
