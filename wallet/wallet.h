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

#pragma once

#include "cclient/api/core/core_api.h"
#include "cclient/api/extended/extended_api.h"
#include "common/model/transaction.h"
#include "wallet_errors.h"

#define IOTA_CLIENT_DEBUG 1
#define DEFAULT_SECURITY_LEVEL 2
#define DEFAULT_TAG "ACCESS999999999999999999999"

/**
 * @brief Wallet context
 *
 */
typedef struct {
  uint8_t mwm;                          /*!< Minimum Weight Magnitude for doing Proof-of-Work */
  uint8_t security;                     /*!< security level of addresses, value could be 1,2,3. */
  uint32_t depth;                       /*!< number of bundles to go back to determine the transactions for approval. */
  uint64_t unused_idx;                  /*!< recent unused index of the address. */
  flex_trit_t seed[FLEX_TRIT_SIZE_243]; /*!< seed */
  iota_client_service_t *iota_client;   /*!< iota client service */
} wallet_ctx_t;

/**
 * @brief Create a wallet instance
 *
 * @param node_url the url of a node
 * @param port_number the port number of a node
 * @param ca_pem a root ca in pem format, pass NULL for HTTP only.
 * @param node_depth the depth of the connected node
 * @param node_mwm the MWM of the connected node
 * @param seed a seed for this wallet
 * @return wallet_ctx_t* return NULL on errors
 */
wallet_ctx_t *wallet_create(char const *const node_url, uint16_t port_number, char const *const ca_pem,
                            uint32_t node_depth, uint8_t node_mwm, char const *const seed);

/**
 * @brief Check balance from a given address
 *
 * @param ctx the wallet context
 * @param address an address fro balance checking
 * @param balance the balance returned from node.
 * @return wallet_err_t error code
 */
wallet_err_t wallet_check_balance(wallet_ctx_t const *const ctx, char const *const address, uint64_t *balance);

/**
 * @brief Send out a valued or policy transaction to the node.
 *
 * @param ctx wallet context
 * @param receiver the receiver address
 * @param balance token value
 * @param msg message in the transaction
 * @return wallet_err_t error code
 */

/**
 * @brief Send out a valued or policy transaction to the node.
 *
 * @param ctx wallet context
 * @param receiver the receiver address
 * @param balance token value
 * @param policy an access policy
 * @param bundle_hash a bundle hash with char[81] is returned by client API.
 * @return wallet_err_t
 */
wallet_err_t wallet_send(wallet_ctx_t const *const ctx, char const *const receiver, uint64_t balance,
                         char const *const policy, char *const bundle_hash);

/**
 * @brief Gets an unused address of this wallet and update unused_idx in wallet context
 *
 * @param ctx wallet context
 * @return wallet_err_t
 */

/**
 * @brief Gets an unused address of this wallet and update unused_idx in wallet context
 *
 * @param ctx wallet context
 * @param addr_buf an address buffer
 * @param index the index of the address
 * @return wallet_err_t
 */
wallet_err_t wallet_get_address(wallet_ctx_t *const ctx, char *addr_buf, uint64_t *index);

/**
 * @brief Clean up and free a wallet instance
 *
 * @param wallet wallet context
 */
void wallet_destory(wallet_ctx_t **wallet);

/**
 * @brief Gets policy from a given hash
 *
 * @param ctx wallet context
 * @param hash a bundle hash
 * @param policy_buf a buffer stores policy
 * @param len the length of policy buffer
 * @return wallet_err_t
 */
wallet_err_t wallet_fetch_policy(wallet_ctx_t const *const ctx, char const *const hash, char *policy_buf, size_t len);

// TODO wallet monitor