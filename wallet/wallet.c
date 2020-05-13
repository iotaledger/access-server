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

#include "wallet.h"
#include "utils/input_validators.h"

wallet_ctx_t *wallet_create(char const *const node_url, uint16_t port_number, char const *const ca_pem,
                            uint32_t node_depth, uint8_t node_mwm, char const *const seed) {
  wallet_ctx_t *ctx = NULL;

  // check parameters
  if (!is_seed((tryte_t *)seed)) {
    printf("Err: Invalid seed! \n");
    return NULL;
  }

  if ((ctx = (wallet_ctx_t *)malloc(sizeof(wallet_ctx_t))) != NULL) {
    if ((ctx->iota_client = iota_client_core_init(node_url, port_number, ca_pem)) == NULL) {
      free(ctx);
      printf("Err: Init iota client failed\n");
      return NULL;
    }
    ctx->depth = node_depth;
    ctx->mwm = node_mwm;
    ctx->security = DEFAULT_SECURITY_LEVEL;
    ctx->unused_idx = 0;
    if (flex_trits_from_trytes(ctx->seed, NUM_TRITS_HASH, seed, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
      printf("Err: converting seed to flex_trit failed\n");
      wallet_destory(&ctx);
      return NULL;
    }

#ifdef IOTA_CLIENT_DEBUG
    logger_helper_init(LOGGER_DEBUG);
    logger_init_client_core(LOGGER_DEBUG);
    logger_init_client_extended(LOGGER_DEBUG);
    logger_init_json_serializer(LOGGER_ERR);
#endif
    return ctx;
  }

  return NULL;
}

void wallet_destory(wallet_ctx_t **wallet) {
  if (wallet && *wallet) {
    if ((*wallet)->iota_client) {
      iota_client_core_destroy(&(*wallet)->iota_client);
#ifdef IOTA_CLIENT_DEBUG
      logger_destroy_client_core();
      logger_destroy_client_extended();
      logger_destroy_json_serializer();
      logger_helper_destroy();
#endif
    }
    free(*wallet);
    *wallet = NULL;
  }
}

wallet_err_t wallet_check_balance(wallet_ctx_t const *const ctx, char const *const address, uint64_t *balance) {
  retcode_t ret_code = RC_OK;
  wallet_err_t err = WALLET_ERR_UNKNOW;
  flex_trit_t tmp_address[FLEX_TRIT_SIZE_243];
  if (ctx == NULL) {
    printf("Err: wallet has not been initiated.\n");
    return WALLET_ERR_INVALID_PRARMS;
  }
  if (!is_address((tryte_t *)address)) {
    printf("Err: Invalided address\n");
    return WALLET_ERR_INVALID_PRARMS;
  }

  get_balances_req_t *balance_req = get_balances_req_new();
  get_balances_res_t *balance_res = get_balances_res_new();

  if (!balance_req || !balance_res) {
    printf("Err: Out of Memory");
    err = WALLET_ERR_OOM;
    goto done;
  }

  if (flex_trits_from_trytes(tmp_address, NUM_TRITS_HASH, address, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Err: converting flex_trit failed\n");
    err = WALLET_ERR_FLEX_TRITS;
    goto done;
  }

  if (get_balances_req_address_add(balance_req, tmp_address) != RC_OK) {
    printf("Err: adding the hash to queue failed.\n");
    goto done;
  }

  balance_req->threshold = 100;

  if ((ret_code = iota_client_get_balances(ctx->iota_client, balance_req, balance_res)) == RC_OK) {
    *balance = get_balances_res_balances_at(balance_res, 0);
    err = WALLET_OK;
  } else {
    printf("Err: get balance failed\n");
  }

done:
  get_balances_req_free(&balance_req);
  get_balances_res_free(&balance_res);
  return err;
}

wallet_err_t wallet_get_address(wallet_ctx_t *const ctx, char *addr_buf, uint64_t *index) {
  retcode_t ret = RC_ERROR;
  hash243_queue_t addresses = NULL;
  address_opt_t opt = {.security = ctx->security, .start = ctx->unused_idx, .total = 0};

  if ((ret = iota_client_get_new_address(ctx->iota_client, ctx->seed, opt, &addresses)) == RC_OK) {
    flex_trits_to_trytes((tryte_t *)addr_buf, NUM_TRYTES_ADDRESS, addresses->prev->hash, NUM_TRITS_ADDRESS,
                         NUM_TRITS_ADDRESS);
    ctx->unused_idx = hash243_queue_count(addresses) - 1;
    *index = ctx->unused_idx;
  } else {
    printf("new address failed: %s\n", error_2_string(ret));
  }
  hash243_queue_free(&addresses);
  return WALLET_OK;
}