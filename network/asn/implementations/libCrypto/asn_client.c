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
 * \file asn_client.c
 * \brief
 * Client side implemntation for ssl based authentication module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "asn_auth.h"
#include "asn_debug.h"
#include "asn_internal.h"
#include "asn_utils.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* AUTH_STAGES */
#define AUTH_ERROR 0
#define AUTH_INIT 1
#define AUTH_GENERATE 2
#define AUTH_VERIFY 3
#define AUTH_FINISH 4
#define AUTH_DONE 5

//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////

/*
 * Stage 0 - Client initialization
 *
 */

static int auth_client_init(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;
  BIGNUM *e = BN_new();
  debug("auth_client_init START");

  /* Generate RSA key */
  ASN_GET_INTERNAL_RSA_C(session) = RSA_new();
  BN_set_word(e, RSA_F4);
  RSA_generate_key_ex(ASN_GET_INTERNAL_RSA_C(session), ASN_RSA_KEY_LEN, e, NULL);

  ASN_GET_INTERNAL_DH(session) = DH_new();

  asnutils_randmem(ASN_GET_INTERNAL_VC(session), ASN_V_STRING_LEN);

  ASN_GET_INTERNAL_OUT_PACKET_COUNT(session) = 1;
  ASN_GET_INTERNAL_IN_PACKET_COUNT(session) = 1;

  memset(ASN_GET_INTERNAL_H(session), 0, RSA_DIGEST_LENGTH);
  memset(ASN_GET_INTERNAL_HC(session), 0, RSA_DIGEST_LENGTH);

  next_stage = AUTH_GENERATE;

  debug("auth_client_init END");

  return next_stage;
}

/*
 * Stage 1 - DH parameter generation
 *
 * Client generates p, g, vc and x.
 * Client calculates e = gx mod p.
 * Client sends ( p || g || e || vc ) to Server.
 *
 */

static inline int auth_client_generate_send(asn_ctx_t *session) {
  int ret = ASN_ERROR;
  const BIGNUM *p = NULL, *g = NULL, *pub_key = NULL;

  DH_get0_pqg(ASN_GET_INTERNAL_DH(session), &p, NULL, &g);
  DH_get0_key(ASN_GET_INTERNAL_DH(session), &pub_key, NULL);

  debug("send p");
  ret = asnutils_send_message_part_bignum(session, p);
  // TODO: Check return

  debug("send g");
  ret = asnutils_send_message_part_bignum(session, g);
  // TODO: Check return

  debug("send e");
  ret = asnutils_send_message_part_bignum(session, pub_key);
  // TODO: Check return

  debug("send vc");
  ret = asnutils_send_message_part(session, ASN_GET_INTERNAL_VC(session), ASN_V_STRING_LEN);
  // TODO: Check return

  return ret;
}

static int auth_client_generate(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;
  BIGNUM *pp, *gg;
  int ret = ASN_ERROR;

  debug("auth_client_generate START");

  if (NULL != ASN_GET_INTERNAL_DH(session)) /* DH parameters generated */
  {
    /* Client generates p, g, vc and x. */
    gg = BN_new();
    if (gg != NULL) {
      BN_set_word(gg, 5);
    }

    pp = BN_get_rfc3526_prime_2048(NULL);
    if (pp == NULL || gg == NULL || !DH_set0_pqg(ASN_GET_INTERNAL_DH(session), pp, NULL, gg)) {
      DH_free(ASN_GET_INTERNAL_DH(session));
      BN_free(pp);
      BN_free(gg);
    } else {
      /* Client calculates e = gx mod p. */
      int ret_ossl = DH_generate_key(ASN_GET_INTERNAL_DH(session));

      if (0 != ret_ossl) /* Keys generated */
      {
        int len = DH_size(ASN_GET_INTERNAL_DH(session));
        ASN_GET_INTERNAL_K(session) = malloc(len);

        if (NULL != ASN_GET_INTERNAL_K(session)) {
          /* Send ( p || g || e || vc ) */
          ret = auth_client_generate_send(session);
          // TODO: Check return

          if (ASN_ERROR != ret) {
            next_stage = AUTH_VERIFY;
          }

          next_stage = AUTH_VERIFY;
        }
      }
    }
  }

  debug("auth_client_generate END");

  return next_stage;
}

/*
 * Stage 2 - Compute and verify Server parameters
 *
 * Client receives ( f || s || ks || vs )
 * Client computes k = fx mod p
 * Client computes h  = hash( vc || vs || ks || e || f || k )
 * Client verifies the signature s on h
 * Client computes hc = hash( vc || vs || kc || e || f || k )
 * Client computes signature sc = sign( skc, hc )
 * Client sends ( kc || sc ) to Server
 *
 */

static inline int auth_client_verify_receive(asn_ctx_t *session, unsigned char **f, unsigned short *cf_len,
                                             unsigned char **s, unsigned short *cs_len, unsigned char **ks,
                                             unsigned short *ks_len, unsigned char **vs, unsigned short *Vs_len) {
  int ret = ASN_ERROR;

  ret = asnutils_receive_message_part(session, f, cf_len);
  // TODO: Check return

  ret = asnutils_receive_message_part(session, s, cs_len);
  // TODO: Check return

  ret = asnutils_receive_message_part(session, ks, ks_len);
  // TODO: Check return

  ret = asnutils_receive_message_part(session, vs, Vs_len);
  // TODO: Check return

  return ret;
}

static inline int auth_client_verify_send(asn_ctx_t *session, unsigned char *kc, unsigned short kc_len,
                                          unsigned char *sc, unsigned short len) {
  int ret = ASN_ERROR;

  debug("send kc");
  ret = asnutils_send_message_part(session, kc, kc_len);
  // TODO: Check return

  debug("send sc");
  ret = asnutils_send_message_part(session, sc, len);
  // TODO: Check return

  return ret;
}

static int auth_client_verify(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;
  int ret;

  unsigned char *kc = NULL, *cKs = NULL, *s = NULL, *cVs = NULL, *cf = NULL;
  unsigned short f_len = 0, cVs_len = 0, s_len = 0, cKs_len = 0;
  const BIGNUM *cpub_key = NULL;
  unsigned int sc_len = 0;
  unsigned char sc[ASN_RSA_SIGN_LEN] = {
      0,
  };
  int kc_len = 0;

  debug("auth_client_verify START");

  // Receive ( f || s || ks || vs )
  ret = auth_client_verify_receive(session, &cf, &f_len, &s, &s_len, &cKs, &cKs_len, &cVs, &cVs_len);
  // TODO: Check return

  memcpy(ASN_GET_INTERNAL_VS(session), cVs, cVs_len);

  ASN_GET_INTERNAL_PUB_K(session) = BN_bin2bn(cf, f_len, NULL);

  /* Server computes k = fx mod p */
  ASN_GET_INTERNAL_K_LEN(session) =
      DH_compute_key(ASN_GET_INTERNAL_K(session), ASN_GET_INTERNAL_PUB_K(session), ASN_GET_INTERNAL_DH(session));

  /* Client checks Server key */
  ret = session->f_verify(cKs, cKs_len);
  // TODO: Check return

  DH_get0_key(ASN_GET_INTERNAL_DH(session), &cpub_key, NULL);
  /* Client computes h  = hash( vc || vs || ks || e || f || k ) */
  ret = asnutils_compute_hash(session, ASN_GET_INTERNAL_H(session), cKs, cKs_len, cpub_key,
                              ASN_GET_INTERNAL_PUB_K(session));

  /* Client verifies the signature s on h */
  ASN_GET_INTERNAL_RSA_S(session) = d2i_RSAPublicKey(NULL, (const unsigned char **)&cKs, cKs_len);
  ret = RSA_verify(/*NID_sha256*/ 0, ASN_GET_INTERNAL_H(session), RSA_DIGEST_LENGTH, s, s_len,
                   ASN_GET_INTERNAL_RSA_S(session));
  debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!RSA_verify (%d)", ret);

  /* Client computes hc = hash( vc || vs || kc || e || f || k ) */
  kc_len = i2d_RSAPublicKey(ASN_GET_INTERNAL_RSA_C(session), &kc);
  ret = asnutils_compute_hash(session, ASN_GET_INTERNAL_HC(session), kc, kc_len, cpub_key,
                              ASN_GET_INTERNAL_PUB_K(session));

  /* Client computes signature sc = sign( skc, hc ) */
  /*NID_sha256*/
  RSA_sign(/*NID_sha256*/ 0, ASN_GET_INTERNAL_HC(session), RSA_DIGEST_LENGTH, sc, &sc_len,
           ASN_GET_INTERNAL_RSA_C(session));
  asnutils_debug_binary("RSA_Client_sign", sc, sc_len);

  // Send ( kc || sc )
  ret = auth_client_verify_send(session, kc, kc_len, sc, sc_len);
  // TODO: Check return

  free(kc);
  free(s);
  free(cVs);
  free(cf);

  next_stage = AUTH_FINISH;

  debug("auth_client_verify END");

  return next_stage;
}

/*
 * Stage 3 - Finish authentication
 *
 * Client generates AES keys.
 * Waits for encryption / decryption tasks :)
 *
 */

static int auth_client_finish(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  debug("auth_client_finish START");

  asnutils_debug_binary("k", ASN_GET_INTERNAL_K(session), ASN_GET_INTERNAL_K_LEN(session));
  asnutils_debug_binary("h", ASN_GET_INTERNAL_H(session), RSA_DIGEST_LENGTH);
  asnutils_debug_binary("hc", ASN_GET_INTERNAL_HC(session), RSA_DIGEST_LENGTH);

  // Generate AES, HMAC keys
  asnutils_generate_keys(session);

  ASN_GET_INTERNAL_KEY_CS(session) = malloc(sizeof(AES_KEY));
  ASN_GET_INTERNAL_KEY_SC(session) = malloc(sizeof(AES_KEY));

  memset(ASN_GET_INTERNAL_KEY_CS(session), 0, sizeof(AES_KEY));
  memset(ASN_GET_INTERNAL_KEY_SC(session), 0, sizeof(AES_KEY));

  // Initialize Client -> Server Encrypt Key
  AES_set_encrypt_key(ASN_GET_INTERNAL_EKEY_C(session), ASN_AES_KEY_LEN, ASN_GET_INTERNAL_KEY_CS(session));

  // Initialize Server -> Client Decrypt Key
  AES_set_decrypt_key(ASN_GET_INTERNAL_EKEY_S(session), ASN_AES_KEY_LEN, ASN_GET_INTERNAL_KEY_SC(session));

  {
    const BIGNUM *priv_key = NULL;
    unsigned char *kc = NULL;
    int kc_len = 0;
    int len = 0;

    DH_get0_key(ASN_GET_INTERNAL_DH(session), NULL, &priv_key);

    len = BN_num_bytes(priv_key);

    unsigned char *data = malloc(len);

    len = BN_bn2bin(priv_key, data);

    asnutils_debug_binary("x", data, len);

    kc_len = i2d_RSAPrivateKey(ASN_GET_INTERNAL_RSA_C(session), &kc);

    asnutils_debug_binary("kc", kc, kc_len);
  }

  next_stage = AUTH_DONE;

  debug("auth_client_finish END");

  return next_stage;
}

/*
 *
 */

int asninternal_client_authenticate(asn_ctx_t *session) {
  int ret = ASN_ERROR;

  int auth_stage = AUTH_INIT;

  debug("asninternal_client_authenticate START");

  while ((AUTH_DONE != auth_stage) && (AUTH_ERROR != auth_stage)) {
    switch (auth_stage) {
      case AUTH_INIT:
        auth_stage = auth_client_init(session);
        break;
      case AUTH_GENERATE:
        auth_stage = auth_client_generate(session);
        break;
      case AUTH_VERIFY:
        auth_stage = auth_client_verify(session);
        break;
      case AUTH_FINISH:
        auth_stage = auth_client_finish(session);
        break;
      default:
        break;
    }
  }

  if (AUTH_DONE == auth_stage) {
    ret = ASN_OK;
  }

  debug("asninternal_client_authenticate END");

  return ret;
}

int asninternal_client_send(asn_ctx_t *session, const unsigned char *data, unsigned short data_len) {
  return asnutils_send(session, ASN_GET_INTERNAL_IKEY_C(session), ASN_GET_INTERNAL_KEY_CS(session),
                       ASN_GET_INTERNAL_IV_C(session), data, data_len);
}

int asninternal_client_receive(asn_ctx_t *session, unsigned char **data, unsigned short *data_len) {
  return asnutils_receive(session, ASN_GET_INTERNAL_IKEY_S(session), ASN_GET_INTERNAL_KEY_SC(session),
                          ASN_GET_INTERNAL_IV_S(session), data, data_len);
}

void asninternal_release_client(asn_ctx_t *session) {}

int asninternal_client_set_option(asn_ctx_t *session, const char *key, unsigned char *value) { return 0; }
