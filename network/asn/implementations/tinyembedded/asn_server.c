/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \file asn_server.c
 * \brief
 * Implementation of server side ASN authentication
 *
 * @Author Dejan Nedic, Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 * 06.12.2018. Implemented ed25519 signature algorithm
 ****************************************************************************/

/////////////////
/// Includes
/////////////////
#include "asn_debug.h"
#include "asn_internal.h"
#include "asn_utils.h"

/////////////////////////////////////
/// Macros and defines
/////////////////////////////////////

/* AUTH_STAGES */
#define AUTH_ERROR 0
#define AUTH_INIT 1
#define AUTH_COMPUTE 2
#define AUTH_VERIFY 3
#define AUTH_FINISH 4
#define AUTH_DONE 5

//////////////////////////////////////////////////////////////////
/// Server authantication function declarations and definitions
//////////////////////////////////////////////////////////////////

int authserver_init(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  unsigned char public[PUBLIC_KEY_L];
  unsigned char private[PRIVATE_KEY_L];
  crypto_sign_keypair(public, private);
  memcpy(ASN_GET_INTERNAL_PUBLIC_KEY(session), public, PUBLIC_KEY_L);
  memcpy(ASN_GET_INTERNAL_PRIVATE_KEY(session), private, PRIVATE_KEY_L);
  memcpy(ASN_GET_INTERNAL_ID_V(session), "server", IDENTIFICATION_STRING_L);

  ASN_GET_INTERNAL_SEQ_NUM_ENCRYPT(session) = 1;
  ASN_GET_INTERNAL_SEQ_NUM_DECRYPT(session) = 1;

  next_stage = AUTH_COMPUTE;

  return next_stage;
}

/* Stage 1 - Compute DH Client parameters
 *
 * Server receives e from Client.
 * Server generates  y.
 * Server calculates f = gy mod p.
 * Server computes k = ey mod p
 * Server computes h  = hash( vc || vs || ks || e || f || k )
 * Server computes signature s = sign( sks, h )
 * Server sends ( ks || f || s )
 *
 * Server receives ( kc || sc )
 * Server computes hc = hash( vc || vs || kc || e || f || k )
 * Server verifies the signature sc on hc
 *
 * */

int authserver_compute(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;
  unsigned char vc[] = {"client"};
  unsigned char s_signed[SIGNED_MESSAGE_L];

  int SIZE_OF_WRITE_BUFFER = PUBLIC_KEY_L + DH_PUBLIC_L + SIGNED_MESSAGE_L;
  unsigned char writeBuffer[SIZE_OF_WRITE_BUFFER];
  unsigned char received_dh_public[DH_PUBLIC_L];

  unsigned char readBuffer[PUBLIC_KEY_L + SIGNED_MESSAGE_L];
  unsigned char *client_public_key;
  unsigned char *signature;

  // Server receives e from Client.
  int read_message = session->f_read(session->ext, received_dh_public, DH_PUBLIC_L);

  // Server generates y and computes f
  int keys_generated = asnutils_dh_generate_keys(session);

  // Server computes k = ey mod p
  int secret_computed = asnutils_dh_compute_secret_k(session, received_dh_public);

  // Server computes h  = hash( vc || vs || ks || e || f || k )
  int h_computed = asnutils_compute_session_identifier_h(
      ASN_GET_INTERNAL_EXCHANGE_HASH(session), vc, ASN_GET_INTERNAL_ID_V(session), ASN_GET_INTERNAL_PUBLIC_KEY(session),
      received_dh_public, ASN_GET_INTERNAL_DH_PUBLIC(session), ASN_GET_INTERNAL_SECREY_K(session));

  // Server computes signature s = sign( sks, h )
  int signature_computed = asnutils_compute_signature_s(s_signed, session, ASN_GET_INTERNAL_EXCHANGE_HASH(session));
  if (signature_computed == 1) {
    dlog_printf("\nERROR signing failed\n");

    return next_stage;
  }

  // Server sends ( ks || f || s )
  asnutils_concatenate_strings(writeBuffer, ASN_GET_INTERNAL_PUBLIC_KEY(session), PUBLIC_KEY_L,
                               ASN_GET_INTERNAL_DH_PUBLIC(session), DH_PUBLIC_L);
  asnutils_concatenate_strings(writeBuffer + PUBLIC_KEY_L + DH_PUBLIC_L, NULL, 0, s_signed, SIGNED_MESSAGE_L);

  int write_message = session->f_write(session->ext, writeBuffer, SIZE_OF_WRITE_BUFFER);

  // Server receives ( kc || sc )
  int read_second_message = session->f_read(session->ext, readBuffer, PUBLIC_KEY_L + SIGNED_MESSAGE_L);
  client_public_key = readBuffer;
  signature = readBuffer + PUBLIC_KEY_L;

  // Server verifies that kc is the public key for Client
  int key_verified = session->f_verify(client_public_key, PUBLIC_KEY_L);

  // Server computes hc = hash( vc || vs || kc || e || f || k )
  int hc_computed = asnutils_compute_session_identifier_h(
      ASN_GET_INTERNAL_EXCHANGE_HASH2(session), vc, ASN_GET_INTERNAL_ID_V(session), client_public_key,
      received_dh_public, ASN_GET_INTERNAL_DH_PUBLIC(session), ASN_GET_INTERNAL_SECREY_K(session));

  // Server verifies the signature sc on hc
  int signature_verified =
      asnutils_verify_signature(signature, client_public_key, ASN_GET_INTERNAL_EXCHANGE_HASH2(session));

  if ((read_message == DH_PUBLIC_L) && (keys_generated == 0) && (secret_computed == 0) && (h_computed == 0) &&
      (signature_computed == 0) && (write_message == SIZE_OF_WRITE_BUFFER) &&
      (read_second_message == PUBLIC_KEY_L + SIGNED_MESSAGE_L) && (key_verified == 0) && (hc_computed == 0) &&
      (signature_verified == 0)) {
    next_stage = AUTH_FINISH;
    dlog_printf("\nAuthentication successful\n");
  } else {
    dlog_printf("\nERROR authentication failed\n");
  }

  return next_stage;
}

/* Stage 2 - Finish authentication
 *
 * Server generates AES keys.
 *
 * */

int authserver_finish(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  int generated = 0;

  // Server generates AES keys.
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_IV_ENCRYPTION(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'B');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_IV_DECRYPTION(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'A');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_DECRYPTION_KEY(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'C');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_ENCRYPTION_KEY(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'D');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_INTEGRITY_KEY_DECRYPTION(session),
                                      ASN_GET_INTERNAL_SECREY_K(session), ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'E');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_INTEGRITY_KEY_ENCRYPTION(session),
                                      ASN_GET_INTERNAL_SECREY_K(session), ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'F');

  if (generated == 0) {
    next_stage = AUTH_DONE;
  }

  return next_stage;
}

int asninternal_server_authenticate(asn_ctx_t *session) {
  int ret = ASN_ERROR;

  int auth_stage = AUTH_INIT;

  while ((AUTH_DONE != auth_stage) && (AUTH_ERROR != auth_stage)) {
    switch (auth_stage) {
      case AUTH_INIT:
        auth_stage = authserver_init(session);
        break;
      case AUTH_COMPUTE:
        auth_stage = authserver_compute(session);
        break;
      case AUTH_FINISH:
        auth_stage = authserver_finish(session);
        ret = 0;
        break;
      default:
        break;
    }
  }

  return ret;
}

int asninternal_server_send(asn_ctx_t *session, const unsigned char *data, unsigned short data_len) {
  return asnutils_write(session, data, data_len);
}

int asninternal_server_receive(asn_ctx_t *session, unsigned char **data, unsigned short *data_len) {
  return asnutils_read(session, data, data_len);
}

void asninternal_release_server(asn_ctx_t *session) {}

int asninternal_server_set_option(asn_ctx_t *session, const char *key, unsigned char *value) {
  return asnutils_set_option(session, key, value);
}
