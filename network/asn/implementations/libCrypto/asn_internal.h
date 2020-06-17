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
 * \file asn_internal.h
 * \brief
 * Header file for ssl based authentication module, internal functions
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

#ifndef ASN_INTERNAL_H_
#define ASN_INTERNAL_H_

#include <openssl/aes.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/hmac.h>
#include <openssl/modes.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>

#include "asn_auth.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* ASN_ERRORS */
#define ASN_TYPE_CLIENT (0)
#define ASN_TYPE_SERVER (1)

#define ASN_MESSAGE_NUMBER_LEN (sizeof(unsigned short))
#define ASN_MESSAGE_BASE_10 (10)

#define ASN_HASH_A ("A")
#define ASN_HASH_B ("B")
#define ASN_HASH_C ("C")
#define ASN_HASH_D ("D")
#define ASN_HASH_E ("E")
#define ASN_HASH_F ("F")

#define ASN_GET_INTERNAL(s) ((s)->internal)
#define ASN_GET_INTERNAL_TYPE(s) (ASN_GET_INTERNAL(s)->type)
#define ASN_GET_INTERNAL_DH(s) (ASN_GET_INTERNAL(s)->asn_dh)
#define ASN_GET_INTERNAL_PUB_K(s) (ASN_GET_INTERNAL(s)->pub_key)
#define ASN_GET_INTERNAL_VC(s) (ASN_GET_INTERNAL(s)->vc)
#define ASN_GET_INTERNAL_VS(s) (ASN_GET_INTERNAL(s)->vs)
#define ASN_GET_INTERNAL_K(s) (ASN_GET_INTERNAL(s)->k)
#define ASN_GET_INTERNAL_K_LEN(s) (ASN_GET_INTERNAL(s)->k_len)
#define ASN_GET_INTERNAL_H(s) (ASN_GET_INTERNAL(s)->h)
#define ASN_GET_INTERNAL_HC(s) (ASN_GET_INTERNAL(s)->hc)
#define ASN_GET_INTERNAL_IV_S(s) (ASN_GET_INTERNAL(s)->iv_s)
#define ASN_GET_INTERNAL_IV_C(s) (ASN_GET_INTERNAL(s)->iv_c)
#define ASN_GET_INTERNAL_EKEY_S(s) (ASN_GET_INTERNAL(s)->ekey_s)
#define ASN_GET_INTERNAL_EKEY_C(s) (ASN_GET_INTERNAL(s)->ekey_c)
#define ASN_GET_INTERNAL_IKEY_S(s) (ASN_GET_INTERNAL(s)->i_key_s)
#define ASN_GET_INTERNAL_IKEY_C(s) (ASN_GET_INTERNAL(s)->i_key_c)
#define ASN_GET_INTERNAL_KEY_CS(s) (ASN_GET_INTERNAL(s)->key_client_server)
#define ASN_GET_INTERNAL_KEY_SC(s) (ASN_GET_INTERNAL(s)->key_server_client)
#define ASN_GET_INTERNAL_RSA_S(s) (ASN_GET_INTERNAL(s)->rsa_server)
#define ASN_GET_INTERNAL_RSA_C(s) (ASN_GET_INTERNAL(s)->rsa_client)
#define ASN_GET_INTERNAL_IN_PACKET_COUNT(s) (ASN_GET_INTERNAL(s)->out_packet_count)
#define ASN_GET_INTERNAL_OUT_PACKET_COUNT(s) (ASN_GET_INTERNAL(s)->in_packet_count)

#define ASN_V_STRING_LEN (16)
#define ASN_AES_DIGEST_LEN (16)
#define ASN_AES_KEY_LEN (256)

#define ASN_RSA_KEY_LEN (1024 * 2)
#define ASN_RSA_SIGN_LEN (ASN_RSA_KEY_LEN / 8)

#define HMAC_DIGEST_LENGTH (SHA256_DIGEST_LENGTH)

#define RSA_DIGEST_LENGTH (SHA256_DIGEST_LENGTH + 4)

struct asn_struct {
  int type; /*ASN_TYPE_X*/

  unsigned char vc[ASN_V_STRING_LEN];
  unsigned char vs[ASN_V_STRING_LEN];

  /* DH parameters */
  DH *asn_dh;
  BIGNUM *pub_key;
  int k_len;
  unsigned char *k;

  /* SHA256 parameters */
  unsigned char h[RSA_DIGEST_LENGTH];
  unsigned char hc[RSA_DIGEST_LENGTH];

  unsigned char iv_s[SHA256_DIGEST_LENGTH];
  unsigned char iv_c[SHA256_DIGEST_LENGTH];

  unsigned char ekey_s[SHA256_DIGEST_LENGTH];
  unsigned char ekey_c[SHA256_DIGEST_LENGTH];

  unsigned char i_key_s[SHA256_DIGEST_LENGTH];
  unsigned char i_key_c[SHA256_DIGEST_LENGTH];

  /* RSA parameters */
  RSA *rsa_client;
  RSA *rsa_server;

  /* AES parameters */
  AES_KEY *key_client_server;
  AES_KEY *key_server_client;

  /* Packet counters */
  unsigned char out_packet_count;
  unsigned char in_packet_count;
};

int asninternal_server_authenticate(asn_ctx_t *);

int asninternal_client_authenticate(asn_ctx_t *);

int asninternal_server_send(asn_ctx_t *, const unsigned char *, unsigned short);

int asninternal_client_send(asn_ctx_t *, const unsigned char *, unsigned short);

int asninternal_server_receive(asn_ctx_t *, unsigned char **, unsigned short *);

int asninternal_client_receive(asn_ctx_t *, unsigned char **, unsigned short *);

void asninternal_release_server(asn_ctx_t *);

void asninternal_release_client(asn_ctx_t *);

int asninternal_server_set_option(asn_ctx_t *, const char *, unsigned char *);

int asninternal_client_set_option(asn_ctx_t *, const char *, unsigned char *);

#endif /* ASN_INTERNAL_H_ */
