/*
 * libdac_internal.h
 *
 *  Created on: Aug 2, 2018
 *      Author: ntt-xain
 */

#ifndef LIBDAC_INTERNAL_H_
#define LIBDAC_INTERNAL_H_

#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/dh.h>

#include "libauthdac.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* DAC_ERRORS */
#define DAC_TYPE_CLIENT          (0)
#define DAC_TYPE_SERVER          (1)

#define DAC_MESSAGE_NUMBER_LEN   (sizeof(unsigned short))
#define DAC_MESSAGE_BASE_10      (10)

#define DAC_HASH_A               ("A")
#define DAC_HASH_B               ("B")
#define DAC_HASH_C               ("C")
#define DAC_HASH_D               ("D")
#define DAC_HASH_E               ("E")
#define DAC_HASH_F               ("F")

#define getInternal(s)         ((s)->internal)
#define getInternalType(s)     (getInternal(s)->type)
#define getInternalDH(s)       (getInternal(s)->dac_dh)
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

#define DAC_V_STRING_LEN       (16)
#define DAC_AES_DIGEST_LEN     (16)
#define DAC_AES_KEY_LEN        (256)

#define DAC_RSA_KEY_LEN        (1024*2)
#define DAC_RSA_SIGN_LEN       (DAC_RSA_KEY_LEN/8)

#define HMAC_DIGEST_LENGTH     (SHA256_DIGEST_LENGTH)

#define RSA_DIGEST_LENGTH      (SHA256_DIGEST_LENGTH+4)

//////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////

struct dacStruct {
   int type; /*DAC_TYPE_X*/

   unsigned char Vc[DAC_V_STRING_LEN];
   unsigned char Vs[DAC_V_STRING_LEN];

   /* DH parameters */
   DH            *dac_dh;
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

/* DAC_ERRORS */int dacServerAuthenticate(dacSession_t *);

/* DAC_ERRORS */int dacClientAuthenticate(dacSession_t *);

/* DAC_ERRORS */int dacSendServer(dacSession_t *, const unsigned char *, unsigned short );

/* DAC_ERRORS */int dacSendClient(dacSession_t *, const unsigned char *, unsigned short );

/* DAC_ERRORS */int dacReceiveServer(dacSession_t *, unsigned char **, unsigned short  *);

/* DAC_ERRORS */int dacReceiveClient(dacSession_t *, unsigned char **, unsigned short  *);

/* DAC_ERRORS */void dacReleaseServer(dacSession_t *);

/* DAC_ERRORS */void dacReleaseClient(dacSession_t *);

int dacServerSetOption(dacSession_t *, const char *, unsigned char *);

int dacClientSetOption(dacSession_t *, const char *, unsigned char *);

#endif /* LIBDAC_INTERNAL_H_ */
