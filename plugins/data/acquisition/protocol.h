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
 * \file protocol.h
 * \brief
 * Implementation of data acquisition wrapper for different com. interfaces
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 18.05.2020. Initial version.
 ****************************************************************************/
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

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

#define PROTOCOL_MAX_STR_LEN 256

/****************************************************************************
 * CALLBACKS
 ****************************************************************************/
typedef int (*acquire_fn)(char* requested_type, char* requested_value, char* type_buff, char* value_buff);
typedef int (*payment_status_fn)(char* policy_id, int policy_id_len);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      PROTOCOL_init
 *
 * @brief   Initialize module
 *
 * @param   wallet_ctx - Device wallet
 *
 * @return  TRUE - success, FALSE - fail
 */
bool PROTOCOL_init(wallet_ctx_t* wallet_ctx);

/**
 * @fn      PROTOCOL_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  TRUE - success, FALSE - fail
 */
bool PROTOCOL_term(void);

/**
 * @fn      PROTOCOL_register_callback
 *
 * @brief   Register callback for different communication protocols
 *
 * @param   acquire - Callback to register
 *
 * @return  void
 */
void PROTOCOL_register_callback(acquire_fn acquire);

/**
 * @fn      PROTOCOL_unregister_callback
 *
 * @brief   Unregister callback for different communication protocols
 *
 * @param   void
 *
 * @return  void
 */
void PROTOCOL_unregister_callback();

/**
 * @fn      PROTOCOL_register_payment_state_callback
 *
 * @brief   Register callback for acquiring payment status
 *
 * @param   trans_fn - Callback for checking payment status
 *
 * @return  void
 */
void PROTOCOL_register_payment_state_callback(payment_status_fn trans_fn);

/**
 * @fn      PROTOCOL_unregister_payment_state_callback
 *
 * @brief   Unregister callback for acquiring payment status
 *
 * @param   void
 *
 * @return  void
 */
void PROTOCOL_unregister_payment_state_callback(void);

#endif //__PROTOCOL_H__
