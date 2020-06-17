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
 * \file asn_client.c
 * \brief
 * Implementation of client side ASN authentication
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
/// Client authantication function declarations and definitions
//////////////////////////////////////////////////////////////////

int auth_client_init(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  unsigned char public[PUBLIC_KEY_L];
  unsigned char private[PRIVATE_KEY_L];

  // generate private and public key on client side
  crypto_sign_keypair(public, private);

  memcpy(ASN_GET_INTERNAL_PUBLIC_KEY(session), public, PUBLIC_KEY_L);
  memcpy(ASN_GET_INTERNAL_PRIVATE_KEY(session), private, PRIVATE_KEY_L);

  memcpy(ASN_GET_INTERNAL_ID_V(session), "client", IDENTIFICATION_STRING_L);
  next_stage = AUTH_COMPUTE;
  return next_stage;
}

/* Stage 1 - DH parameter generation
 *
 * Client generates x and calculates e = gx mod p.
 * Client sends e to Server.
 *
 * */

int auth_client_generate(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  // Client generates p, g, vc and x and calculates e = gx mod p.
  int keys_generated = asnutils_dh_generate_keys(session);

  // Client sends e to Server.
  int write_message = session->f_write(session->ext, ASN_GET_INTERNAL_DH_PUBLIC(session), DH_PUBLIC_L);

  ASN_GET_INTERNAL_SEQ_NUM_ENCRYPT(session) = 1;
  ASN_GET_INTERNAL_SEQ_NUM_DECRYPT(session) = 1;

  if ((keys_generated == 0) && (write_message == DH_PUBLIC_L)) {
    next_stage = AUTH_VERIFY;
  }

  return next_stage;
}

/* Stage 2 - Compute and verify Server parameters
 *
 * Client receives ( ks || f || s )
 * Client computes k = fx mod p
 * Client computes h  = hash( vc || vs || ks || e || f || k )
 * Client verifies the signature s on h
 * Client computes hc = hash( vc || vs || kc || e || f || k )
 * Client computes signature sc = sign( skc, hc )
 * Client sends ( kc || sc ) to Server
 *
 * */

int auth_client_verify(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  int SIZE_OF_READ_BUFFER = PUBLIC_KEY_L + DH_PUBLIC_L + SIGNED_MESSAGE_L;  // size of ks + f + s
  unsigned char readBuffer[SIZE_OF_READ_BUFFER];
  unsigned char *s_signed;
  unsigned char *received_dh_public;
  unsigned char *server_public_key;

  unsigned char vs[IDENTIFICATION_STRING_L];
  memcpy(vs, "server", IDENTIFICATION_STRING_L);
  unsigned char signature[SIGNED_MESSAGE_L];
  unsigned char message[PUBLIC_KEY_L + SIGNED_MESSAGE_L];

  // Client receives ( ks || f || s )
  ssize_t read_message = session->f_read(session->ext, readBuffer, SIZE_OF_READ_BUFFER);
  if (read_message != SIZE_OF_READ_BUFFER) {
    return AUTH_ERROR;
  }

  server_public_key = readBuffer;
  received_dh_public = readBuffer + PUBLIC_KEY_L;
  s_signed = readBuffer + PUBLIC_KEY_L + DH_PUBLIC_L;

  // Client verifies that ks is public key of the Server
  int key_verified = session->f_verify(received_dh_public, PUBLIC_KEY_L);

  // Client computes k = fx mod p
  int secret_computed = asnutils_dh_compute_secret_k(session, received_dh_public);

  // Client computes h  = hash( vc || vs || ks || e || f || k )
  int h_computed = asnutils_compute_session_identifier_h(
      ASN_GET_INTERNAL_EXCHANGE_HASH(session), ASN_GET_INTERNAL_ID_V(session), vs, server_public_key,
      ASN_GET_INTERNAL_DH_PUBLIC(session), received_dh_public, ASN_GET_INTERNAL_SECREY_K(session));

  // Client verifies the signature s on h
  int signature_verified =
      asnutils_verify_signature(s_signed, server_public_key, ASN_GET_INTERNAL_EXCHANGE_HASH(session));

  // Client computes hc = hash( vc || vs || kc || e || f || k )
  int computed_H = asnutils_compute_session_identifier_h(
      ASN_GET_INTERNAL_EXCHANGE_HASH2(session), ASN_GET_INTERNAL_ID_V(session), vs,
      ASN_GET_INTERNAL_PUBLIC_KEY(session), ASN_GET_INTERNAL_DH_PUBLIC(session), received_dh_public,
      ASN_GET_INTERNAL_SECREY_K(session));

  // Client computes signature sc = sign( skc, hc )
  int message_signed = asnutils_compute_signature_s(signature, session, ASN_GET_INTERNAL_EXCHANGE_HASH2(session));

  asnutils_concatenate_strings(message, ASN_GET_INTERNAL_PUBLIC_KEY(session), PUBLIC_KEY_L, signature,
                               SIGNED_MESSAGE_L);
  ssize_t message_written = session->f_write(session->ext, message, PUBLIC_KEY_L + SIGNED_MESSAGE_L);

  if ((read_message == SIZE_OF_READ_BUFFER) && (key_verified == 0) && (secret_computed == 0) && (h_computed == 0) &&
      (signature_verified == 0) && (computed_H == 0) && (message_signed == 0) &&
      (message_written == PUBLIC_KEY_L + SIGNED_MESSAGE_L)) {
    next_stage = AUTH_FINISH;
  } else {
    printf("\n%d\n", read_message);
    printf("%d\n", key_verified);
    printf("%d\n", secret_computed);
    printf("%d\n", h_computed);
    printf("%d\n", signature_verified);
    printf("%d\n", computed_H);
    printf("%d\n", message_signed);
    printf("%d\n", message_written);
  }

  return next_stage;
}

/* Stage 3 - Finish authentication
 *
 * Client generates AES keys.
 * Waits for encryption / decryption tasks :)
 *
 * */

int auth_client_finish(asn_ctx_t *session) {
  int next_stage = AUTH_ERROR;

  int generated = 0;

  // Client generates AES keys.
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_IV_ENCRYPTION(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'A');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_IV_DECRYPTION(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'B');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_ENCRYPTION_KEY(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'C');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_DECRYPTION_KEY(session), ASN_GET_INTERNAL_SECREY_K(session),
                                      ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'D');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_INTEGRITY_KEY_ENCRYPTION(session),
                                      ASN_GET_INTERNAL_SECREY_K(session), ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'E');
  generated +=
      asnutils_generate_enc_auth_keys(ASN_GET_INTERNAL_INTEGRITY_KEY_DECRYPTION(session),
                                      ASN_GET_INTERNAL_SECREY_K(session), ASN_GET_INTERNAL_EXCHANGE_HASH(session), 'F');

  if (generated == 0) {
    next_stage = AUTH_DONE;
  }

  return next_stage;
}

int asninternal_client_authenticate(asn_ctx_t *session) {
  int ret = ASN_ERROR;

  int auth_stage = AUTH_INIT;
  while ((AUTH_DONE != auth_stage) && (AUTH_ERROR != auth_stage)) {
    switch (auth_stage) {
      case AUTH_INIT:
        auth_stage = auth_client_init(session);
        break;
      case AUTH_COMPUTE:
        auth_stage = auth_client_generate(session);
        break;
      case AUTH_VERIFY:
        auth_stage = auth_client_verify(session);
        break;
      case AUTH_FINISH:
        auth_stage = auth_client_finish(session);
        ret = 0;
        break;
      default:
        break;
    }
  }

  return ret;
}

int asninternal_client_send(asn_ctx_t *session, const unsigned char *data, unsigned short data_len) {
  return asnutils_write(session, data, data_len);
}

int asninternal_client_receive(asn_ctx_t *session, unsigned char **data, unsigned short *data_len) {
  return asnutils_read(session, data, data_len);
}

void asninternal_release_client(asn_ctx_t *session) {}

int asninternal_client_set_option(asn_ctx_t *session, const char *key, unsigned char *value) {
  return asnutils_set_option(session, key, value);
}
