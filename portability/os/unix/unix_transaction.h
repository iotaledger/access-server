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
 * \project IOTA Access
 * \file unix_trans.h
 * \brief
 * Implementation of API for transactions storage to unix
 *
 * @Author Strahinja Golic, Bernardo Araujo.
 *
 * \notes
 *
 * \history
 * 10.06.2020. Initial version.
 * 28.07.2020. Renaming.
 ****************************************************************************/
#ifndef _UNIX_TRANSACTION_H_
#define _unix_TRANSACTION_H_

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

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      unix_transaction_store
 *
 * @brief   Store trnasaction on unix
 *
 * @param   policy_id - Policy ID string
 * @param   policy_id_len - Length of policy ID string
 *
 * @return  TRUE - success, FALSE - fail
 */
bool unix_transaction_store(char* policy_id, int policy_id_len);

/**
 * @fn      unix_transaction_store
 *
 * @brief   Store transaction on unix
 *
 * @param   policy_id - Policy ID string
 * @param   policy_id_len - Length of policy ID string
 * @param   is_verified - Transaction verified
 *
 * @return  TRUE - success, FALSE - fail
 */
bool unix_transaction_update_payment_status(char* policy_id, int policy_id_len, bool is_verified);

/**
 * @fn      unix_transaction_is_stored
 *
 * @brief   Check if transaction is stored on unix
 *
 * @param   policy_id - Policy ID string
 *
 * @return  TRUE - stored, FALSE - not stored
 */
bool unix_transaction_is_stored(char* policy_id);

/**
 * @fn      unix_transaction_is_verified
 *
 * @brief   Check if transaction payment is verified
 *
 * @param   policy_id - Policy ID string
 * @param   policy_id_len - Length of policy ID string
 *
 * @return  TRUE - verified, FALSE - not verified
 */
bool unix_transaction_is_verified(char* policy_id, int policy_id_len);

#endif  //_unix_TRANSACTION_H_
