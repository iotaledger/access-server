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
 * \file libdacUtils.h
 * \brief
 * Header file with function definitions for dac authentication.
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 14.08.2018. Initial version.
 ****************************************************************************/
#ifndef LIBDACUTILS_H_
#define LIBDACUTILS_H_

#include <stdlib.h>

#include "libdac_internal.h"

/**
 * @fn  int compute_signature_s(unsigned char *sig, dacSession_t *dacSession, unsigned char *hash);
 *
 * @brief   Function that signes data with private key
 *
 * @param   sig           Result of signing function
 * @param   dacSession    Data structure that contains session related information
 * @param   hash          Computed hash H for signing
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int compute_signature_s(unsigned char *sig, dacSession_t *dacSession, unsigned char *hash);

/**
 * @fn  int verify_signature(unsigned char *sig, unsigned char *public_key, unsigned char *hash);
 *
 * @brief   Function that verifies signed date with public key
 *
 * @param   sig          Signature to be verified
 * @param   public_key   Public key for signature verification
 * @param   hash         Computed hash H for signing
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int verify_signature(unsigned char *sig, unsigned char *public_key, unsigned char *hash);

/**
 * @fn  int dh_generate_keys(dacSession_t *dacSession);
 *
 * @brief   Function that generates Diffie-Hellman private and public keys
 *
 * @param   dacSession    Data structure that contain private and public Diffie-Hellman keys
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dh_generate_keys(dacSession_t *dacSession);

/**
 * @fn  int dh_compute_secret_K(dacSession_t *dacSession,  const unsigned char *public_key);
 *
 * @brief   Function that computes shared secret from Diffie-Hellman key exchange
 *
 * @param   dacSession    Data structure that contain session related data
 * @param   public_key    Received public key
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dh_compute_secret_K(dacSession_t *dacSession,  const unsigned char *public_key);

/**
 * @fn  int compute_session_identifier_H(unsigned char *exchange_hash, char *Vc, char *Vs, char *Ks, unsigned char *c_public, unsigned char *s_public, unsigned char *secretK);
 *
 * @brief   Function that computes hash H from Diffie-Hellman key exchange
 *
 * @param   exchange_hash    Shared hash H
 * @param   Vc          Client idetification string
 * @param   Vs          Server idetification string
 * @param   K           Public key
 * @param   c_public    Client Diffie-Hellman public key
 * @param   s_public    Server Diffie-Hellman public key
 * @param   secretK     Shared secret K
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int compute_session_identifier_H(unsigned char *exchange_hash, unsigned char *Vc, unsigned char *Vs, unsigned char *K, unsigned char *c_public, unsigned char *s_public, unsigned char *secretK);

/**
 * @fn  int generate_enc_auth_keys(unsigned char *hash, char *shared_secret_K, char *shared_H, char magic_letter);
 *
 * @brief   Function that computes AES keys
 *
 * @param   hash               Computed AES key
 * @param   shared_secret_K    Shared secret K
 * @param   shared_H           Shared hash H
 * @param   magic_letter       Character, unique for different key types ('A' - 'F')
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int generate_enc_auth_keys(unsigned char *hash, unsigned char *shared_secret_K, unsigned char *shared_H, char magic_letter);

/**
 * @fn  int concatinate_strings(unsigned char *concatinatedString, unsigned char *str1, int str1_l, unsigned char * str2, int str2_l);
 *
 * @brief   Function that takes two strings and concatinates them
 *
 * @param   concatinatedString    Concatinated string
 * @param   str1                  First string to be concatinated
 * @param   str1_l                First string length
 * @param   str2                  Second string to be concatinated
 * @param   str2_l                Second string length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int concatinate_strings(unsigned char *concatinatedString, unsigned char *str1, int str1_l, unsigned char * str2, int str2_l);

/**
 * @fn  int dacUtilsRead(dacSession_t *session, unsigned char **msg, int length);
 *
 * @brief   Function that reads messages
 *
 * @param   session   Data structure that contain session related data
 * @param   msg       Buffer for message to be read
 * @param   length    Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacUtilsRead(dacSession_t *session, unsigned char **msg, unsigned short *messageLength);

/**
 * @fn  int dacUtilsWrite(dacSession_t *session, unsigned char *msg, int length);
 *
 * @brief   Function that writes messages
 *
 * @param   session   Data structure that contain session related data
 * @param   msg       Message to be written
 * @param   length    Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacUtilsWrite(dacSession_t *session, const unsigned char *msg, unsigned short messageLength);

/**
 * @fn  int dacUtilSetOption(dacSession_t *session, const char *key, unsigned char *value)
 *
 * @brief   Function that writes messages
 *
 * @param   session   	Data structure that contain session related data
 * @param   key       	Option to be set
 * @param   value    	Value for the option
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacUtilSetOption(dacSession_t *session, const char *key, unsigned char *value);

#endif /* LIBDACUTILS_H_ */
