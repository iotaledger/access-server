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

#include <inttypes.h>

#include "utils/input_validators.h"
#include "wallet.h"
#include "wallet_logger.h"

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
      log_error(wallet_logger_id, "[%s:%d] Init iota client failed.\n", __func__, __LINE__);

      printf("Err: \n");
      return NULL;
    }
    ctx->depth = node_depth;
    ctx->mwm = node_mwm;
    ctx->security = DEFAULT_SECURITY_LEVEL;
    ctx->unused_idx = 0;
    if (flex_trits_from_trytes(ctx->seed, NUM_TRITS_HASH, seed, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
      log_error(wallet_logger_id, "[%s:%d] Converting seed to flex_trit failed.\n", __func__, __LINE__);
      wallet_destroy(&ctx);
      return NULL;
    }

    logger_helper_init(LOGGER_DEBUG);
    logger_init_client_core(LOGGER_DEBUG);
    logger_init_client_extended(LOGGER_DEBUG);
    logger_init_json_serializer(LOGGER_ERR);
    logger_init_wallet(LOGGER_DEBUG);

    char *int2str = malloc(8);
    log_debug(wallet_logger_id, "[%s:%d] wallet initialized.\n", __func__, __LINE__);
    snprintf(int2str, 2, "%d", node_depth);
    log_debug(wallet_logger_id, "[%s:%d] wallet depth: %s\n", __func__, __LINE__, int2str);
    snprintf(int2str, 3, "%d", node_mwm);
    log_debug(wallet_logger_id, "[%s:%d] wallet mwm: %s\n", __func__, __LINE__, int2str);
    snprintf(int2str, 8, "%d", port_number);
    log_debug(wallet_logger_id, "[%s:%d] wallet node url: %s:%s\n", __func__, __LINE__, node_url, int2str);

    free(int2str);

    return ctx;
  }

  return NULL;
}

void wallet_destroy(wallet_ctx_t **wallet) {
  if (wallet && *wallet) {
    if ((*wallet)->iota_client) {
      iota_client_core_destroy(&(*wallet)->iota_client);
#ifdef IOTA_CLIENT_DEBUG
      logger_destroy_client_core();
      logger_destroy_client_extended();
      logger_destroy_json_serializer();
      logger_destroy_wallet();
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
    log_error(wallet_logger_id, "[%s:%d] Wallet has not been initialized.\n", __func__, __LINE__);
    return WALLET_ERR_PRARMS;
  }
  if (!is_address((tryte_t *)address)) {
    log_error(wallet_logger_id, "[%s:%d] Invalided address.\n", __func__, __LINE__);
    printf("Err: \n");
    return WALLET_ERR_PRARMS;
  }

  get_balances_req_t *balance_req = get_balances_req_new();
  get_balances_res_t *balance_res = get_balances_res_new();

  if (!balance_req || !balance_res) {
    log_error(wallet_logger_id, "[%s:%d] Out of Memory.\n", __func__, __LINE__);
    err = WALLET_ERR_OOM;
    goto done;
  }

  if (flex_trits_from_trytes(tmp_address, NUM_TRITS_HASH, address, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    log_error(wallet_logger_id, "[%s:%d] Converting flex_trit failed.\n", __func__, __LINE__);
    err = WALLET_ERR_FLEX_TRITS;
    goto done;
  }

  if (get_balances_req_address_add(balance_req, tmp_address) != RC_OK) {
    log_error(wallet_logger_id, "[%s:%d] Adding the hash to queue failed.\n", __func__, __LINE__);
    goto done;
  }

  balance_req->threshold = 100;

  if ((ret_code = iota_client_get_balances(ctx->iota_client, balance_req, balance_res)) == RC_OK) {
    *balance = get_balances_res_balances_at(balance_res, 0);
    err = WALLET_OK;
  } else {
    log_error(wallet_logger_id, "[%s:%d] Get balance failed.\n", __func__, __LINE__);
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
    log_error(wallet_logger_id, "[%s:%d] New address failed: %s.\n", __func__, __LINE__, error_2_string(ret));
  }
  hash243_queue_free(&addresses);
  return WALLET_OK;
}

wallet_err_t wallet_send(wallet_ctx_t const *const ctx, char const *const receiver, uint64_t balance,
                         char const *const msg, char *const bundle_hash) {
  retcode_t client_ret = RC_ERROR;
  wallet_err_t ret = WALLET_ERR_UNKNOW;
  // check parameters
  if (!is_address((tryte_t *)receiver)) {
    log_error(wallet_logger_id, "[%s:%d] Invalid receiver address.\n", __func__, __LINE__);
    return WALLET_ERR_PRARMS;
  }

  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  transfer_array_t *transfers = transfer_array_new();

  /* transfer setup */
  transfer_t tf = {};

  // receiver
  if (flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, (tryte_t const *)receiver, NUM_TRYTES_ADDRESS,
                             NUM_TRYTES_ADDRESS) == 0) {
    log_error(wallet_logger_id, "[%s:%d] Address flex_trits convertion failed.\n", __func__, __LINE__);
    ret = WALLET_ERR_FLEX_TRITS;
    goto done;
  }

  // tag
  if (flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, (tryte_t const *)DEFAULT_TAG, NUM_TRYTES_TAG, NUM_TRYTES_TAG) ==
      0) {
    log_error(wallet_logger_id, "[%s:%d] Tag flex_trits convertion failed.\n", __func__, __LINE__);
    ret = WALLET_ERR_FLEX_TRITS;
    goto done;
  }

  // value
  tf.value = balance;

  // message (optional)
  transfer_message_set_string(&tf, msg);

  transfer_array_add(transfers, &tf);

  client_ret = iota_client_send_transfer(ctx->iota_client, ctx->seed, ctx->security, ctx->depth, ctx->mwm, false,
                                         transfers, NULL, NULL, NULL, bundle);

  log_info(wallet_logger_id, "[%s:%d] Send transaction: %s.\n", __func__, __LINE__, error_2_string(client_ret));
  if (client_ret == RC_OK) {
    flex_trit_t const *hash = bundle_transactions_bundle_hash(bundle);
    flex_trits_to_trytes((tryte_t *)bundle_hash, NUM_TRYTES_BUNDLE, hash, NUM_TRITS_BUNDLE, NUM_TRITS_BUNDLE);

    // printf("bundle hash: ");
    // flex_trit_print(hash, NUM_TRITS_BUNDLE);
    // printf("\n");
    ret = WALLET_OK;
  } else {
    log_error(wallet_logger_id, "[%s:%d] Send transaction failed.\n", __func__, __LINE__);
    ret = WALLET_ERR_CLIENT_ERR;
  }

done:
  bundle_transactions_free(&bundle);
  transfer_message_free(&tf);
  transfer_array_free(transfers);
  return ret;
}

static void *balance_service(void *arg) {
  balance_service_t *serv = (balance_service_t *)arg;
  wallet_err_t err = WALLET_ERR_UNKNOW;

  if ((err = wallet_check_balance(serv->wallet, serv->addr, &serv->current_balance)) == WALLET_OK) {
    uint64_t old_balance = serv->current_balance;
    serv->status = 1;
    log_info(wallet_logger_id, "[%s:%d] [%ld] Balance service start at balance %" PRIu64 "\n", __func__, __LINE__,
             pthread_self(), serv->current_balance);
    while ((serv->current_balance - old_balance) < serv->threshold) {
      sleep(serv->interval);
      if (serv->status == 2) {
        log_info(wallet_logger_id, "[%s:%d] [%ld] Balance service stop.\n", __func__, __LINE__, pthread_self());
        break;
      }

      if ((err = wallet_check_balance(serv->wallet, serv->addr, &serv->current_balance)) != WALLET_OK) {
        log_info(wallet_logger_id, "[%s:%d] [%ld] Balance service get balance failed.\n", __func__, __LINE__,
                 pthread_self());
        break;
      }
      log_info(wallet_logger_id,
               "[%s:%d] [%ld] Starting: %\" PRIu64 \", current :%\" PRIu64 \", threshold: %\" PRIu64 \"\n", __func__,
               __LINE__, pthread_self(), old_balance, serv->current_balance, serv->threshold);
    }

    if (serv->cb) {
      serv->cb(old_balance, serv->current_balance);
    }

  } else {
    log_info(wallet_logger_id, "[%s:%d] [%ld] Init balace service failed.\n", __func__, __LINE__, pthread_self());
    printf("[%ld] Err: \n", pthread_self());
  }

  log_info(wallet_logger_id, "[%s:%d] [%ld] terminating :%\" PRIu64 \"\n", __func__, __LINE__, pthread_self(),
           serv->current_balance);
}

balance_service_t *balance_service_start(wallet_ctx_t const *wallet, char const *const address, uint32_t interval,
                                         uint64_t threshold, balance_cb cb) {
  // init a balance context
  balance_service_t *serv = (balance_service_t *)malloc(sizeof(balance_service_t));
  memcpy(serv->addr, address, NUM_TRYTES_ADDRESS);
  serv->interval = interval;
  serv->threshold = threshold;
  serv->current_balance = 0;
  serv->status = 0;
  serv->wallet = wallet;
  serv->cb = cb;

  if (pthread_mutex_init(&serv->mutex_lock, NULL) != 0) {
    log_error(wallet_logger_id, "[%s:%d] Init locker failed.\n", __func__, __LINE__);
    free(serv);
    return NULL;
  }

  if (pthread_create(&serv->thread_id, NULL, balance_service, (void *)serv) != 0) {
    log_error(wallet_logger_id, "[%s:%d] Thread creation failed.\n", __func__, __LINE__);
    free(serv);
    return NULL;
  }

  return serv;
}

void balance_service_stop(balance_service_t *serv) {
  pthread_mutex_lock(&serv->mutex_lock);
  serv->status = 2;
  pthread_mutex_unlock(&serv->mutex_lock);
}

void balance_service_free(balance_service_t **serv) {
  if (serv && *serv) {
    free(*serv);
    *serv = NULL;
  }
}

wallet_err_t wallet_fetch_msg(wallet_ctx_t const *const ctx, char const *const hash, char *msg_buf, size_t len) {
  // TODO
  return 0;
}

bool wallet_check_confirmation(wallet_ctx_t const *const ctx, char const *const tx_hash) {
  retcode_t client_ret = RC_ERROR;
  bool is_confirmed = false;

  flex_trit_t flex_tx[FLEX_TRIT_SIZE_243];
  get_inclusion_states_req_t *get_inclusion_req = get_inclusion_states_req_new();
  get_inclusion_states_res_t *get_inclusion_res = get_inclusion_states_res_new();
  if (!get_inclusion_req || !get_inclusion_res) {
    log_error(wallet_logger_id, "[%s:%d] OOM.\n", __func__, __LINE__);
    goto done;
  }

  if (flex_trits_from_trytes(flex_tx, NUM_TRITS_HASH, tx_hash, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    log_error(wallet_logger_id, "[%s:%d] Converting flex_trit failed.\n", __func__, __LINE__);
    goto done;
  }

  if ((client_ret = get_inclusion_states_req_hash_add(get_inclusion_req, flex_tx)) != RC_OK) {
    log_error(wallet_logger_id, "[%s:%d] Adding hash failed.\n", __func__, __LINE__);
    goto done;
  }

  if ((client_ret = iota_client_get_inclusion_states(ctx->iota_client, get_inclusion_req, get_inclusion_res)) ==
      RC_OK) {
    is_confirmed = get_inclusion_states_res_states_at(get_inclusion_res, 0);
  } else {
    log_error(wallet_logger_id, "[%s:%d] %s.\n", __func__, __LINE__, error_2_string(client_ret));
  }

done:
  get_inclusion_states_req_free(&get_inclusion_req);
  get_inclusion_states_res_free(&get_inclusion_res);
  return is_confirmed;
}

static void *confirmation_service(void *arg) {
  confirmation_service_t *serv = (confirmation_service_t *)arg;
  bool is_confirmed = false;
  uint32_t time = 0;

  if ((is_confirmed = wallet_check_confirmation(serv->wallet, serv->tx)) == false) {
    while (1) {
      if (time >= serv->timeout) {
        log_error(wallet_logger_id, "[%s:%d] [%ld] Confirmation service stop due to timeout.\n", __func__, __LINE__,
                  pthread_self());
        break;
      }

      if (serv->status == 2) {
        log_error(wallet_logger_id, "[%s:%d] [%ld] Confirmation service stop.\n", __func__, __LINE__, pthread_self());
        break;
      }

      if (is_confirmed = wallet_check_confirmation(serv->wallet, serv->tx) == true) {
        log_info(wallet_logger_id, "[%s:%d] [%ld] Transaction has been confirmed.\n", __func__, __LINE__,
                 pthread_self());
        break;
      }

      time += serv->interval;
      // printf("[%ld] is_confirmed %d\n", pthread_self(), is_confirmed);
      sleep(serv->interval);
    };
  }

  if (serv->cb) {
    serv->cb(time, is_confirmed, serv->thread_id);
  }
  log_info(wallet_logger_id, "[%s:%d] [%ld] Confirmation service terminating.\n", __func__, __LINE__, pthread_self());
}

confirmation_service_t *confirmation_service_start(wallet_ctx_t const *wallet, char const *const tx_hash,
                                                   uint32_t interval, uint32_t timeout, confirmed_cb cb) {
  // init service context
  confirmation_service_t *serv = (confirmation_service_t *)calloc(1, sizeof(confirmation_service_t));
  memcpy(serv->tx, tx_hash, NUM_TRYTES_HASH);
  serv->interval = interval;
  serv->timeout = timeout;
  serv->status = 0;
  serv->wallet = wallet;
  serv->cb = cb;

  if (pthread_mutex_init(&serv->mutex_lock, NULL) != 0) {
    log_error(wallet_logger_id, "[%s:%d] Init locker failed.\n", __func__, __LINE__);
    free(serv);
    return NULL;
  }

  if (pthread_create(&serv->thread_id, NULL, confirmation_service, (void *)serv) != 0) {
    log_error(wallet_logger_id, "[%s:%d] Thread creation failed.\n", __func__, __LINE__);
    free(serv);
    return NULL;
  }

  return serv;
}

void confirmation_service_stop(confirmation_service_t *serv) {
  pthread_mutex_lock(&serv->mutex_lock);
  serv->status = 2;
  pthread_mutex_unlock(&serv->mutex_lock);
}

void confirmation_service_free(confirmation_service_t **serv) {
  if (serv && *serv) {
    free(*serv);
    *serv = NULL;
  }
}
