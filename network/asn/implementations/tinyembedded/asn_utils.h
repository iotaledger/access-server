/****************************************************************************
 *  Copyright (c) 2018 XAIN
 *
 *  All Rights Reserved
 *
 *  P R O P R I E T A R Y    &    C O N F I D E N T I A L
 *
 *  -----------------------------------------------------
 *  https://xain.io/
 *  -----------------------------------------------------
 *
 * \project Decentralized Access Control
 * \file asn_utils.h
 * \brief
 * Header file with function definitions for ASN authentication.
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 14.08.2018. Initial version.
 ****************************************************************************/
#ifndef ASN_UTILS_H_
#define ASN_UTILS_H_

#include <stdlib.h>

#include "asn_internal.h"

/**
 * @fn  int asnutils_compute_signature_s(unsigned char *sig, asn_ctx_t
 * *session, unsigned char *hash);
 *
 * @brief   Function that signes data with private key
 *
 * @param   sig           Result of signing function
 * @param   session    Data structure that contains session related information
 * @param   hash          Computed hash h for signing
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_compute_signature_s(unsigned char *sig, asn_ctx_t *session, unsigned char *hash);

/**
 * @fn  int asnutils_verify_signature(unsigned char *sig, unsigned char
 * *public_key, unsigned char *hash);
 *
 * @brief   Function that verifies signed date with public key
 *
 * @param   sig          Signature to be verified
 * @param   public_key   Public key for signature verification
 * @param   hash         Computed hash h for signing
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_verify_signature(unsigned char *sig, unsigned char *public_key, unsigned char *hash);

/**
 * @fn  int asnutils_dh_generate_keys(asn_ctx_t *session);
 *
 * @brief   Function that generates Diffie-Hellman private and public keys
 *
 * @param   session    Data structure that contain private and public
 * Diffie-Hellman keys
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_dh_generate_keys(asn_ctx_t *session);

/**
 * @fn  int asnutils_dh_compute_secret_k(asn_ctx_t *session,  const unsigned
 * char *public_key);
 *
 * @brief   Function that computes shared secret from Diffie-Hellman key
 * exchange
 *
 * @param   session    Data structure that contain session related data
 * @param   public_key    Received public key
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_dh_compute_secret_k(asn_ctx_t *session, const unsigned char *public_key);

/**
 * @fn  int asnutils_compute_session_identifier_h(unsigned char *exchange_hash,
 * char *vc, char *vs, char *ks, unsigned char *c_public, unsigned char
 * *s_public, unsigned char *secretK);
 *
 * @brief   Function that computes hash h from Diffie-Hellman key exchange
 *
 * @param   exchange_hash    Shared hash h
 * @param   vc          Client idetification string
 * @param   vs          Server idetification string
 * @param   k           Public key
 * @param   c_public    Client Diffie-Hellman public key
 * @param   s_public    Server Diffie-Hellman public key
 * @param   secretK     Shared secret k
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_compute_session_identifier_h(unsigned char *exchange_hash, unsigned char *vc, unsigned char *vs,
                                          unsigned char *k, unsigned char *c_public, unsigned char *s_public,
                                          unsigned char *secretK);

/**
 * @fn  int asnutils_generate_enc_auth_keys(unsigned char *hash, char
 * *shared_secret_K, char *shared_H, char magic_letter);
 *
 * @brief   Function that computes AES keys
 *
 * @param   hash               Computed AES key
 * @param   shared_secret_K    Shared secret k
 * @param   shared_H           Shared hash h
 * @param   magic_letter       Character, unique for different key types ('A' -
 * 'F')
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_generate_enc_auth_keys(unsigned char *hash, unsigned char *shared_secret_K, unsigned char *shared_H,
                                    char magic_letter);

/**
 * @fn  int asnutils_concatenate_strings(unsigned char *concatenatedString,
 * unsigned char *str1, int str1_l, unsigned char * str2, int str2_l);
 *
 * @brief   Function that takes two strings and concatenates them
 *
 * @param   concatenatedString    Concatinated string
 * @param   str1                  First string to be concatenated
 * @param   str1_l                First string length
 * @param   str2                  Second string to be concatenated
 * @param   str2_l                Second string length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_concatenate_strings(unsigned char *concatenatedString, unsigned char *str1, int str1_l,
                                 unsigned char *str2, int str2_l);

/**
 * @fn  int asnutils_read(asn_ctx_t *session, unsigned char **msg, int
 * length);
 *
 * @brief   Function that reads messages
 *
 * @param   session   Data structure that contain session related data
 * @param   msg       Buffer for message to be read
 * @param   length    Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_read(asn_ctx_t *session, unsigned char **msg, unsigned short *message_length);

/**
 * @fn  int asnutils_write(asn_ctx_t *session, unsigned char *msg, int
 * length);
 *
 * @brief   Function that writes messages
 *
 * @param   session   Data structure that contain session related data
 * @param   msg       Message to be written
 * @param   length    Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_write(asn_ctx_t *session, const unsigned char *msg, unsigned short message_length);

/**
 * @fn  int asnutils_set_option(asn_ctx_t *session, const char *key,
 * unsigned char *value)
 *
 * @brief   Function that writes messages
 *
 * @param   session   	Data structure that contain session related data
 * @param   key       	Option to be set
 * @param   value    	Value for the option
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int asnutils_set_option(asn_ctx_t *session, const char *key, unsigned char *value);

#endif /* ASN_UTILS_H_ */
