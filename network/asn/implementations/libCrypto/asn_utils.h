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

//#define NDEBUG

#include "asn_debug.h"

#include "asn_internal.h"

void asnUtils_randmem(unsigned char *randomString, int length);

int asnUtils_send_message_part(asnSession_t *session, void *data, unsigned short dataLen);

int asnUtils_send_message_part_bignum(asnSession_t *session, const BIGNUM *bn);

int asnUtils_receive_message_part(asnSession_t *session, unsigned char **data, unsigned short *dataLen);

int asnUtils_compute_hash(asnSession_t *session, unsigned char *md,
      unsigned char *pKey, int pKey_len, const BIGNUM *e, const BIGNUM *pub_key);

void asnUtils_debug_binary(char *name, unsigned char* data, int len);

void asnUtils_generate_keys(asnSession_t *session);

/* ASN_ERRORS */int asnUtils_send(asnSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv, const unsigned char *data, unsigned short  data_len);

/* ASN_ERRORS */int asnUtils_receive(asnSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv, unsigned char **data, unsigned short  *data_len);

#endif /* ASN_UTILS_H_ */
