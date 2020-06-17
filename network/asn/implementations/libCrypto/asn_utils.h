/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \file asn_utils.h
 * \brief
 * Header file for ssl based authentication module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

#ifndef ASN_UTILS_H_
#define ASN_UTILS_H_

#include <stdlib.h>
#include <string.h>

#include "asn_debug.h"
#include "asn_internal.h"

void asnutils_randmem(unsigned char *randomString, int length);

int asnutils_send_message_part(asn_ctx_t *session, void *data, unsigned short data_len);

int asnutils_send_message_part_bignum(asn_ctx_t *session, const BIGNUM *bn);

int asnutils_receive_message_part(asn_ctx_t *session, unsigned char **data, unsigned short *data_len);

int asnutils_compute_hash(asn_ctx_t *session, unsigned char *md, unsigned char *pkey, int pkey_len, const BIGNUM *e,
                          const BIGNUM *pub_key);

void asnutils_debug_binary(char *name, unsigned char *data, int len);

void asnutils_generate_keys(asn_ctx_t *session);

int asnutils_send(asn_ctx_t *session, const void *hmacKey, AES_KEY *aes_key, unsigned char *iv,
                  const unsigned char *data, unsigned short data_len);

int asnutils_receive(asn_ctx_t *session, const void *hmacKey, AES_KEY *aes_key, unsigned char *iv, unsigned char **data,
                     unsigned short *data_len);

#endif /* ASN_UTILS_H_ */
