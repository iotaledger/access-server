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
 * \file rpi_trans.h
 * \brief
 * Implementation of API for transactions storage to RPI
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 10.06.2020. Initial version.
 ****************************************************************************/
#ifndef _RPI_TRANSACTION_H_
#define _RPI_TRANSACTION_H_

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
 * @fn      RPITRANSACTION_store
 *
 * @brief   Store trnasaction on RPI
 *
 * @param   user_id - User ID string
 * @param   user_id_len - Length of user ID string
 * @param   action - Action string
 * @param   action_len - Length of action string
 *
 * @return  TRUE - success, FALSE - fail
 */
bool RPITRANSACTION_store(char* user_id, int user_id_len,
							char* action, int action_len);

/**
 * @fn      RPITRANSACTION_store
 *
 * @brief   Store trnasaction on RPI
 *
 * @param   user_id - User ID string
 * @param   user_id_len - Length of user ID string
 * @param   action - Action string
 * @param   action_len - Length of action string
 * @param   is_verified - Transaction verified
 *
 * @return  TRUE - success, FALSE - fail
 */
bool RPITRANSACTION_update_payment_status(char* user_id, int user_id_len,
											char* action, int action_len, bool is_verified);

#endif //_RPI_TRANSACTION_H_