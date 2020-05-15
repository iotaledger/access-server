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

#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/dh.h>

#include "asn_auth.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* ASN_ERRORS */
#define ASN_TYPE_CLIENT          (0)
#define ASN_TYPE_SERVER          (1)

#define ASN_MESSAGE_NUMBER_LEN   (sizeof(unsigned short))
#define ASN_MESSAGE_BASE_10      (10)

#define ASN_HASH_A               ("A")
#define ASN_HASH_B               ("B")
#define ASN_HASH_C               ("C")
#define ASN_HASH_D               ("D")
#define ASN_HASH_E               ("E")
#define ASN_HASH_F               ("F")

#define getInternal(s)         ((s)->internal)
#define getInternalType(s)     (getInternal(s)->type)
#define getInternalDH(s)       (getInternal(s)->asn_dh)
#define getInternalPubK(s)     (getInternal(s)->pub_key)
#define getInternalVc(s)       (getInternal(s)->Vc)
#define getInternalVs(s)       (getInternal(s)->Vs)
#define getInternalK(s)        (getInternal(s)->K)
#define getInternalK_len(s)    (getInternal(s)->K_len)
#define getInternalH(s)        (getInternal(s)->H)
#define getInternalHc(s)       (getInternal(s)->Hc)
#define getInternalIV_S(s)     (getInternal(s)->iv_s)
#define getInternalIV_C(s)     (getInternal(s)->iv_c)
#define getInternalEKey_S(s)   (getInternal(s)->ekey_s)
#define getInternalEKey_C(s)   (getInternal(s)->ekey_c)
#define getInternalIKey_S(s)   (getInternal(s)->i_key_s)
#define getInternalIKey_C(s)   (getInternal(s)->i_key_c)
#define getInternalKey_CS(s)   (getInternal(s)->keyClientServer)
#define getInternalKey_SC(s)   (getInternal(s)->keyServerClient)
#define getInternalRSA_S(s)    (getInternal(s)->rsaServer)
#define getInternalRSA_C(s)    (getInternal(s)->rsaClient)
#define getInternalInP_Count(s)    (getInternal(s)->out_packet_count)
#define getInternalOutP_Count(s)   (getInternal(s)->in_packet_count)

#define ASN_V_STRING_LEN       (16)
#define ASN_AES_DIGEST_LEN     (16)
#define ASN_AES_KEY_LEN        (256)

#define ASN_RSA_KEY_LEN        (1024*2)
#define ASN_RSA_SIGN_LEN       (ASN_RSA_KEY_LEN/8)

#define HMAC_DIGEST_LENGTH     (SHA256_DIGEST_LENGTH)

#define RSA_DIGEST_LENGTH      (SHA256_DIGEST_LENGTH+4)

//////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////

struct asnStruct {
   int type; /*ASN_TYPE_X*/

   unsigned char Vc[ASN_V_STRING_LEN];
   unsigned char Vs[ASN_V_STRING_LEN];

   /* DH parameters */
   DH            *asn_dh;
   BIGNUM        *pub_key;
   int            K_len;
   unsigned char *K;

   /* SHA256 parameters */
   unsigned char H[RSA_DIGEST_LENGTH];
   unsigned char Hc[RSA_DIGEST_LENGTH];

   unsigned char iv_s[SHA256_DIGEST_LENGTH];
   unsigned char iv_c[SHA256_DIGEST_LENGTH];

   unsigned char ekey_s[SHA256_DIGEST_LENGTH];
   unsigned char ekey_c[SHA256_DIGEST_LENGTH];

   unsigned char i_key_s[SHA256_DIGEST_LENGTH];
   unsigned char i_key_c[SHA256_DIGEST_LENGTH];

   /* RSA parameters */
   RSA *rsaClient;
   RSA *rsaServer;

   /* AES parameters */
   AES_KEY *keyClientServer;
   AES_KEY *keyServerClient;

   /* Packet counters */
   unsigned char out_packet_count;
   unsigned char in_packet_count;
};

//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////

/* ASN_ERRORS */int asnInternal_server_authenticate(asnSession_t *);

/* ASN_ERRORS */int asnInternal_client_authenticate(asnSession_t *);

/* ASN_ERRORS */int asnInternal_server_send(asnSession_t *, const unsigned char *, unsigned short );

/* ASN_ERRORS */int asnInternal_client_send(asnSession_t *, const unsigned char *, unsigned short );

/* ASN_ERRORS */int asnInternal_server_receive(asnSession_t *, unsigned char **, unsigned short  *);

/* ASN_ERRORS */int asnInternal_client_receive(asnSession_t *, unsigned char **, unsigned short  *);

/* ASN_ERRORS */void asnInternal_release_server(asnSession_t *);

/* ASN_ERRORS */void asnInternal_release_client(asnSession_t *);

int asnInternal_server_set_option(asnSession_t *, const char *, unsigned char *);

int asnInternal_client_set_option(asnSession_t *, const char *, unsigned char *);

#endif /* ASN_INTERNAL_H_ */
