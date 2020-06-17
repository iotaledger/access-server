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
 * \file asn_utils.c
 * \brief
 * Implementation of key exchange and encryption key computation
 *
 * @Author Dejan Nedic, Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 * 06.12.2018. Implemented ed25519 signature algorithm
 ****************************************************************************/
#include "asn_utils.h"

#include "dlog.h"

#define ENC_DATA_LEN 2
#define SEC_NUM_LEN 1
#define READ_BUF_LEN 5
#define CHARS_TO_READ 3

//////////////////////////////////////
///
//////////////////////////////////////

int asnutils_dh_generate_keys(asn_ctx_t *session) {
  static const unsigned char basepoint[DH_PRIVATE_L] = {9};
  int r = rand();

  // TODO: From where this magic numbers came from?
  memset(ASN_GET_INTERNAL_DH_PRIVATE(session), r, DH_PRIVATE_L);
  ASN_GET_INTERNAL_DH_PRIVATE(session)[0] &= 248;
  ASN_GET_INTERNAL_DH_PRIVATE(session)[31] &= 127;
  ASN_GET_INTERNAL_DH_PRIVATE(session)[31] |= 64;

  curve25519_donna(ASN_GET_INTERNAL_DH_PUBLIC(session), ASN_GET_INTERNAL_DH_PRIVATE(session), basepoint);

  return 0;
}

int asnutils_dh_compute_secret_k(asn_ctx_t *session, const unsigned char *public_key) {
  curve25519_donna(ASN_GET_INTERNAL_SECREY_K(session), ASN_GET_INTERNAL_DH_PRIVATE(session), public_key);

  return 0;
}

static int hash(unsigned char *exchange_hash, unsigned char *message, int message_length) {
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, message, message_length);
  sha256_final(&ctx, exchange_hash);

  return 0;
}

int asnutils_compute_session_identifier_h(unsigned char *exchange_hash, unsigned char *vc, unsigned char *vs,
                                          unsigned char *k, unsigned char *c_public, unsigned char *s_public,
                                          unsigned char *secretK) {
  int CONCATINATED_STRING_L = 2 * IDENTIFICATION_STRING_L + PUBLIC_KEY_L + 2 * DH_PUBLIC_L + DH_SHARED_SECRET_L;
  int CURENT_LENGTH = 0;
  unsigned char concatenated_string[CONCATINATED_STRING_L];

  for (int i = 0; i < IDENTIFICATION_STRING_L; i++) {
    concatenated_string[i] = vc[i];
  }

  CURENT_LENGTH += IDENTIFICATION_STRING_L;
  for (int i = 0; i < IDENTIFICATION_STRING_L; i++) {
    concatenated_string[CURENT_LENGTH + i] = vs[i];
  }

  CURENT_LENGTH += IDENTIFICATION_STRING_L;
  for (int i = 0; i < PUBLIC_KEY_L; i++) {
    concatenated_string[CURENT_LENGTH + i] = k[i];
  }

  CURENT_LENGTH += PUBLIC_KEY_L;
  for (int i = 0; i < DH_PUBLIC_L; i++) {
    concatenated_string[CURENT_LENGTH + i] = c_public[i];
  }

  CURENT_LENGTH += DH_PUBLIC_L;
  for (int i = 0; i < DH_PUBLIC_L; i++) {
    concatenated_string[CURENT_LENGTH + i] = s_public[i];
  }

  CURENT_LENGTH += DH_PUBLIC_L;
  for (int i = 0; i < DH_SHARED_SECRET_L; i++) {
    concatenated_string[CURENT_LENGTH + i] = secretK[i];
  }

  hash(exchange_hash, concatenated_string, CONCATINATED_STRING_L);

  return 0;
}

int asnutils_generate_enc_auth_keys(unsigned char *hash, unsigned char *shared_secret_K, unsigned char *shared_H,
                                    char magic_letter) {
  SHA256_CTX ctx;
  unsigned char contatinated_string[DH_SHARED_SECRET_L + EXCHANGE_HASH_L + 1];

  for (int i = 0; i < DH_SHARED_SECRET_L; i++) {
    contatinated_string[i] = shared_secret_K[i];
  }

  for (int i = 0; i < EXCHANGE_HASH_L; i++) {
    contatinated_string[DH_SHARED_SECRET_L + i] = shared_H[i];
  }

  contatinated_string[DH_SHARED_SECRET_L + EXCHANGE_HASH_L] = magic_letter;

  sha256_init(&ctx);
  sha256_update(&ctx, contatinated_string, sizeof(contatinated_string));
  sha256_final(&ctx, hash);

  return 0;
}

int asnutils_compute_signature_s(unsigned char *sig, asn_ctx_t *session, unsigned char *hash) {
  unsigned long long smlen;

  crypto_sign(sig, &smlen, hash, DH_SHARED_SECRET_L, ASN_GET_INTERNAL_PRIVATE_KEY(session));

  dlog_printf("\nSMLEN: %d", smlen);

  return 0;
}

int asnutils_verify_signature(unsigned char *sig, unsigned char *public_key, unsigned char *hash) {
  unsigned long long mlen;
  int ret = crypto_sign_open(hash, &mlen, sig, SIGNED_MESSAGE_L, public_key);

  if (ret == -1) {
    dlog_printf("\nVerification failed!\n");
  }

  return -ret;
}

int asnutils_concatenate_strings(unsigned char *concatenatedString, unsigned char *str1, int str1_l,
                                 unsigned char *str2, int str2_l) {
  for (int i = 0; i < str1_l; i++) {
    concatenatedString[i] = str1[i];
  }

  for (int i = 0; i < str2_l; i++) {
    concatenatedString[str1_l + i] = str2[i];
  }

  return 0;
}

static int aes_encrypt(AES_ctx_t *ctx, unsigned char *message, int length) {
  AES_CBC_encrypt_buffer(ctx, message, length);

  return 0;
}

static int aes_decrypt(AES_ctx_t *ctx, unsigned char *message, int length) {
  AES_CBC_decrypt_buffer(ctx, message, length);

  return 0;
}

static void hmac_sha256(unsigned char *mac, unsigned char *integrityKey, uint16_t keyLength, unsigned char *message,
                        uint32_t message_length) {
  SHA256_CTX ss;
  unsigned char kh[HASH_OUTPUT_L];
  unsigned char key[keyLength];
  unsigned char *internal_key;
  size_t internal_key_l = 0;

  memcpy(key, integrityKey, keyLength);

  if (keyLength > SHA256_BLOCK_BYTES) {
    sha256_init(&ss);
    sha256_update(&ss, key, keyLength);
    sha256_final(&ss, kh);

    internal_key = kh;
    internal_key_l = HASH_OUTPUT_L;
  } else {
    internal_key = key;
    internal_key_l = keyLength;
  }

  unsigned char kx[SHA256_BLOCK_BYTES];
  for (size_t i = 0; i < internal_key_l; i++) {
    kx[i] = I_PAD ^ internal_key[i];
  }
  for (size_t i = internal_key_l; i < SHA256_BLOCK_BYTES; i++) {
    kx[i] = I_PAD ^ 0;
  }

  sha256_init(&ss);
  sha256_update(&ss, kx, SHA256_BLOCK_BYTES);
  sha256_update(&ss, message, message_length);
  sha256_final(&ss, mac);

  for (size_t i = 0; i < internal_key_l; i++) {
    kx[i] = O_PAD ^ internal_key[i];
  }
  for (size_t i = internal_key_l; i < SHA256_BLOCK_BYTES; i++) {
    kx[i] = O_PAD ^ 0;
  }

  sha256_init(&ss);
  sha256_update(&ss, kx, SHA256_BLOCK_BYTES);
  sha256_update(&ss, mac, HASH_OUTPUT_L);
  sha256_final(&ss, mac);
}

int asnutils_write(asn_ctx_t *session, const unsigned char *msg, unsigned short message_length) {
  // TODO: From where this magic numbers came from?
  unsigned short encrypted_data_length =
      ((message_length + 2 + 15) / 16) * 16;  // determine size of encrypted data with padding
  unsigned short buffer_length = encrypted_data_length + MAC_HASH_L + ENC_DATA_LEN + SEC_NUM_LEN;
  unsigned char mac[MAC_HASH_L];

  unsigned char buffer[buffer_length];

  buffer[0] = ASN_GET_INTERNAL_SEQ_NUM_ENCRYPT(session);
  buffer[1] = ((encrypted_data_length >> 8));
  buffer[2] = (encrypted_data_length);
  buffer[3] = ((message_length >> 8));
  buffer[4] = message_length;

  for (int i = 0; i < message_length; i++) {
    buffer[i + 5] = msg[i];
  }

  for (int i = message_length + 2; i < encrypted_data_length; i++) {
    buffer[i + 3] = 0;
  }

  aes_encrypt(&ASN_GET_INTERNAL_CTX_ENCRYPT(session), buffer + 3, encrypted_data_length);

  hmac_sha256(mac, ASN_GET_INTERNAL_INTEGRITY_KEY_ENCRYPTION(session), INTEGRITY_KEY_L, buffer,
              encrypted_data_length + 3);

  for (int i = 0; i < MAC_HASH_L; i++) {
    buffer[i + encrypted_data_length + 2 + 1] = mac[i];
  }

  int n = session->f_write(session->ext, buffer, buffer_length);

  ASN_GET_INTERNAL_SEQ_NUM_ENCRYPT(session)++;

  if (n <= 0) {
    return 1;
  }

  return 0;
}

int asnutils_read(asn_ctx_t *session, unsigned char **msg, unsigned short *message_length) {
  unsigned short encrypted_data_length = 0;

  unsigned char sequence_number = 0;

  unsigned char received_mac[MAC_HASH_L];
  unsigned char mac[MAC_HASH_L];

  unsigned char buffer[READ_BUF_LEN];
  unsigned char *encrypted_msg_buffer;

  session->f_read(session->ext, buffer, CHARS_TO_READ);

  sequence_number = buffer[0];

  if (sequence_number != ASN_GET_INTERNAL_SEQ_NUM_DECRYPT(session)) {
    return 1;
  }
  encrypted_data_length = buffer[1];
  encrypted_data_length *= 256;
  encrypted_data_length += buffer[2];

  encrypted_msg_buffer = malloc(encrypted_data_length + 3);

  if (NULL == encrypted_msg_buffer) {
    dlog_printf("MALLOC failed encrypted_msg_buffer");
    return 1;
  }

  encrypted_msg_buffer[0] = buffer[0];
  encrypted_msg_buffer[1] = buffer[1];
  encrypted_msg_buffer[2] = buffer[2];

  session->f_read(session->ext, encrypted_msg_buffer + 3, encrypted_data_length);

  session->f_read(session->ext, received_mac, MAC_HASH_L);

  hmac_sha256(mac, ASN_GET_INTERNAL_INTEGRITY_KEY_DECRYPTION(session), INTEGRITY_KEY_L, encrypted_msg_buffer,
              encrypted_data_length + 3);

  if (memcmp(mac, received_mac, MAC_HASH_L) == 0) {
    aes_decrypt(&ASN_GET_INTERNAL_CTX_DECRYPT(session), encrypted_msg_buffer + 3, encrypted_data_length);

    *message_length = encrypted_msg_buffer[3];
    *message_length *= 256;
    *message_length += encrypted_msg_buffer[4];

    int i = 0;
    unsigned char *ss = malloc(*message_length);

    if (NULL != ss) {
      for (i = 0; i < *message_length; i++) {
        ss[i] = encrypted_msg_buffer[i + 5];
      }

      free(encrypted_msg_buffer);
      *msg = ss;
      ASN_GET_INTERNAL_SEQ_NUM_DECRYPT(session)++;
    } else {
      dlog_printf("ERROR with malloc ss!");
      return 1;
    }
  } else {
    dlog_printf("Data integrity not confirmed");
    return 1;
  }

  return 0;
}

int asnutils_set_option(asn_ctx_t *session, const char *key, unsigned char *value) {
  int ret = ASN_ERROR;

  if ((strlen(key) == strlen("private")) && (0 == memcmp(key, "private", strlen("private")))) {
    memcpy(ASN_GET_INTERNAL_PRIVATE_KEY(session), value, PRIVATE_KEY_L);
    ret = ASN_OK;
  } else if ((strlen(key) == strlen("public")) && (0 == memcmp(key, "public", strlen("public")))) {
    memcpy(ASN_GET_INTERNAL_PUBLIC_KEY(session), value, PUBLIC_KEY_L);
    ret = ASN_OK;
  }

  return ret;
}
