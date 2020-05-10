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
 * \file libdacClient.c
 * \brief
 * Server side implemntation for ssl based authentication module
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

//#define NDEBUG

#include "dacdbg.h"

#include "libauthdac.h"

#include "libdac_internal.h"
#include "libdacUtils.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* AUTH_STAGES */
#define AUTH_ERROR   0
#define AUTH_INIT    1
#define AUTH_COMPUTE 2
#define AUTH_VERIFY  3
#define AUTH_FINISH  4
#define AUTH_DONE    5
//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////

/*
 * Stage 0 - Server initialization
 *
 */

/* AUTH_STAGES */int authServerInit(dacSession_t *session)
{
   int next_stage = AUTH_ERROR;
   BIGNUM *e = BN_new();
   debug("authServerInit START");

   /* Generate RSA key */
   getInternalRSA_S(session) = RSA_new();
   BN_set_word(e, RSA_F4);
   RSA_generate_key_ex(getInternalRSA_S(session), DAC_RSA_KEY_LEN, e, NULL);

   getInternalDH(session) = DH_new();

   /* Server generates Vs, and y. */
   randmem(getInternalVs(session), DAC_V_STRING_LEN);

   getInternalOutP_Count(session) = 1;
   getInternalInP_Count(session) = 1;

   memset(getInternalH(session), 0, RSA_DIGEST_LENGTH);
   memset(getInternalHc(session), 0, RSA_DIGEST_LENGTH);

   next_stage = AUTH_COMPUTE;

   debug("authServerInit END");

   return next_stage;
}

/* Stage 1 - Compute DH Client parameters
 *
 * Server receives ( p || g || e || Vc ) from Client.
 * Server generates Vs, and y.
 * Server calculates f = gy mod p.
 * Server computes K = ey mod p
 * Server computes H  = hash( Vc || Vs || Ks || e || f || K )
 * Server computes signature s = sign( sks, H )
 * Server sends ( f || s || Ks || Vs )
 *
 */

static inline int authServerComputeReceive(dacSession_t *session,
      unsigned char **p, unsigned short *p_len,
      unsigned char **g, unsigned short *g_len,
      unsigned char **e, unsigned short *e_len,
      unsigned char **Vc, unsigned short *Vc_len)
{
   int ret = DAC_ERROR;

   ret = ReceiveMessagePart(session, p, p_len);
   //TODO: Check return

   ret = ReceiveMessagePart(session, g, g_len);
   //TODO: Check return

   ret = ReceiveMessagePart(session, e, e_len);
   //TODO: Check return

   ret = ReceiveMessagePart(session, Vc, Vc_len);
   //TODO: Check return

   return ret;
}

static inline int authServerComputeSend(dacSession_t *session, const BIGNUM *f,
      unsigned char *s, unsigned short s_len,
      unsigned char *Ks, unsigned short Ks_len)
{
   int ret = DAC_ERROR;

   debug("send f");
   ret = SendMessagePartBN(session, f);
   //TODO: Check return

   debug("send s");
   ret = SendMessagePart(session, s, s_len);
   //TODO: Check return

   debug("send Ks");
   ret = SendMessagePart(session, Ks, Ks_len);
   //TODO: Check return

   debug("send Vs");
   ret = SendMessagePart(session, getInternalVs(session), DAC_V_STRING_LEN);
   //TODO: Check return

   return ret;
}

static inline int authServerComputePopulateServerKeys(dacSession_t *session, BIGNUM *p, BIGNUM *g, BIGNUM *pub_key)
{
   int ret = DAC_ERROR;
   int ret_ossl;

   if (NULL != getInternalDH(session)) /* DH parameters generated */
   {
      /* Set parameters from Client */
      ret_ossl = DH_set0_pqg(getInternalDH(session), p, NULL, g);

      if (0 != ret_ossl)
      {
         /* Server calculates f = gy mod p. */
         ret_ossl = DH_generate_key(getInternalDH(session));

         if (0 != ret_ossl) /* Keys generated */
         {
            int len = DH_size(getInternalDH(session));
            getInternalK(session) = malloc(len);

            if(NULL != getInternalK(session))
            {
               /* Server computes K = ey mod p */
               getInternalK_len(session) = DH_compute_key(getInternalK(session),
                     pub_key, getInternalDH(session));

               ret = DAC_OK;
            }
         }
      }
   }

   return ret;
}

/* AUTH_STAGES */int authServerCompute(dacSession_t *session)
{
   int next_stage = AUTH_ERROR;
   int ret = DAC_ERROR;

   unsigned char *sp = NULL, *sg = NULL, *se = NULL, *Vc = NULL, *Ks = NULL;
   unsigned short sp_len = 0, sg_len = 0, se_len = 0, Vc_len = 0, Ks_len = 0;
   BIGNUM *p = NULL, *g = NULL;
   const BIGNUM *spub_key = NULL;
   unsigned int s_len = 0;
   unsigned char s[DAC_RSA_SIGN_LEN] = { 0, };

   debug("authServerCompute START");

   // Receive ( p || g || e || Vc )
   ret = authServerComputeReceive(session, &sp, &sp_len, &sg, &sg_len, &se, &se_len, &Vc, &Vc_len);
   //TODO: Check return

   memcpy(getInternalVc(session), Vc, Vc_len);

   dacDebugBinary("p", sp, sp_len);

   p = BN_bin2bn(sp, sp_len, NULL);

   dacDebugBinary("g", sg, sg_len);

   g = BN_bin2bn(sg, sg_len, NULL);

   dacDebugBinary("e", se, se_len);

   getInternalPubK(session) = BN_bin2bn(se, se_len, NULL);

   ret = authServerComputePopulateServerKeys(session, p, g, getInternalPubK(session));
   //TODO: Check return

   DH_get0_key(getInternalDH(session), &spub_key, NULL);

   /* Server computes H  = hash( Vc || Vs || Ks || e || f || K ) */
   Ks_len = i2d_RSAPublicKey(getInternalRSA_S(session), &Ks);
   ret = ComputeHash(session, getInternalH(session), Ks, Ks_len,
         getInternalPubK(session), spub_key);

   /* Server computes signature s = sign( sks, H ) */
   /*NID_sha256*/
   RSA_sign(/*NID_sha256*/ 0, getInternalH(session), RSA_DIGEST_LENGTH, s, &s_len, getInternalRSA_S(session));
   dacDebugBinary("RSA_Server_sign", s, s_len);

   /* Server sends ( f || s || Ks || Vs ) */
   ret = authServerComputeSend(session, spub_key, s, s_len, Ks, Ks_len);
   //TODO: Check return

   free(sp);
   free(sg);
   free(se);
   free(Vc);
   free(Ks);

   next_stage = AUTH_VERIFY;

   debug("authServerCompute END");

   return next_stage;
}

/* Stage 2 - Verify Client parameters
 *
 * Server receives ( Kc || sc )
 * Server computes Hc = hash( Vc || Vs || Kc || e || f || K )
 * Server verifies the signature sc on Hc
 *
 */

static inline int authServerVerifyReceive(dacSession_t *session,
      unsigned char **Kc, unsigned short *Kc_len,
      unsigned char **sc, unsigned short *sc_len)
{
   int ret = DAC_ERROR;

   ret = ReceiveMessagePart(session, Kc, Kc_len);
   //TODO: Check return

   ret = ReceiveMessagePart(session, sc, sc_len);
   //TODO: Check return

   return ret;
}

/* AUTH_STAGES */int authServerVerify(dacSession_t *session)
{
   int next_stage = AUTH_ERROR;
   int ret = DAC_ERROR;

   unsigned char *Kc = NULL, *s = NULL;
   unsigned short Kc_len = 0, s_len = 0;
   const BIGNUM *spub_key = NULL;

   debug("authServerVerify START");

   // Receive ( Kc || sc )
   ret = authServerVerifyReceive(session, &Kc, &Kc_len, &s, &s_len);
   //TODO: Check return

   /* Server checks Client key */
   ret = session->f_verify(Kc, Kc_len);
   //TODO: Check return

   DH_get0_key(getInternalDH(session), &spub_key, NULL);
   /* Server computes Hc = hash( Vc || Vs || Kc || e || f || K ) */
   ret = ComputeHash(session, getInternalHc(session), Kc, Kc_len,
         getInternalPubK(session), spub_key);

   /* Server verifies the signature sc on Hc */
   getInternalRSA_C(session) = d2i_RSAPublicKey(NULL, (const unsigned char **) &Kc, Kc_len);
   ret = RSA_verify(/*NID_sha256*/ 0, getInternalHc(session), RSA_DIGEST_LENGTH, s, s_len, getInternalRSA_C(session));
   debug("!!!!!!!!!!!!!!!!!!!!!!!!!!!RSA_verify (%d)", ret);
   dacDebugBinary("RSA_Client_sign", s, s_len);

   free(s);

   next_stage = AUTH_FINISH;

   debug("authServerVerify END");

   return next_stage;
}

/* Stage 3 - Finish authentication
 *
 * Server generates AES keys.
 * Waits for encryption / decryption tasks :)
 *
 */

/* AUTH_STAGES */int authServerFinish(dacSession_t *session)
{
   int next_stage = AUTH_ERROR;

   debug("authServerFinish START");

   dacDebugBinary("K", getInternalK(session), getInternalK_len(session));
   dacDebugBinary("H", getInternalH(session), RSA_DIGEST_LENGTH);
   dacDebugBinary("Hc", getInternalHc(session), RSA_DIGEST_LENGTH);

   // Generate AES, HMAC keys
   dacGenerateKeys(session);

   getInternalKey_CS(session) = malloc(sizeof(AES_KEY));
   getInternalKey_SC(session) = malloc(sizeof(AES_KEY));

   memset(getInternalKey_CS(session), 0, sizeof(AES_KEY));
   memset(getInternalKey_SC(session), 0, sizeof(AES_KEY));

   // Initialize Server -> Client Encrypt Key
   AES_set_encrypt_key(getInternalEKey_S(session), DAC_AES_KEY_LEN, getInternalKey_SC(session));

   // Initialize Client -> Server Decrypt Key
   AES_set_decrypt_key(getInternalEKey_C(session), DAC_AES_KEY_LEN, getInternalKey_CS(session));

   {
      const BIGNUM *priv_key = NULL;
      unsigned char *Ks = NULL;
      int Ks_len = 0;
      int len = 0;

      DH_get0_key(getInternalDH(session), NULL, &priv_key);

      len = BN_num_bytes(priv_key);

      unsigned char *data = malloc(len);

      len = BN_bn2bin(priv_key, data);

      dacDebugBinary("y", data, len);

      Ks_len = i2d_RSAPrivateKey(getInternalRSA_S(session), &Ks);

      dacDebugBinary("Ks", Ks, Ks_len);
   }

   next_stage = AUTH_DONE;

   debug("authServerFinish END");

   return next_stage;
}

/*
 *
 */

/* DAC_ERRORS */int dacServerAuthenticate(dacSession_t *session)
{
   int ret = DAC_ERROR;

   int authStage = AUTH_INIT;

   debug("dacServerAuthenticate START");

   while ((AUTH_DONE != authStage) && (AUTH_ERROR != authStage))
   {
      switch (authStage) {
         case AUTH_INIT:
            authStage = authServerInit(session);
            break;
         case AUTH_COMPUTE:
            authStage = authServerCompute(session);
            break;
         case AUTH_VERIFY:
            authStage = authServerVerify(session);
            break;
         case AUTH_FINISH:
            authStage = authServerFinish(session);
            break;
         default:
            break;
      }
   }

   if(AUTH_DONE == authStage)
   {
      ret = DAC_OK;
   }

   debug("dacServerAuthenticate END");

   return ret;
}

/* DAC_ERRORS */int dacSendServer(dacSession_t *session, const unsigned char *data, unsigned short  data_len)
{
   return dacInternalSend(session, getInternalIKey_S(session), getInternalKey_SC(session),
         getInternalIV_S(session), data, data_len);
}

/* DAC_ERRORS */int dacReceiveServer(dacSession_t *session, unsigned char **data, unsigned short  *data_len)
{
   return dacInternalReceive(session, getInternalIKey_C(session), getInternalKey_CS(session),
         getInternalIV_C(session), data, data_len);
}

void dacReleaseServer(dacSession_t *session)
{

}

int dacServerSetOption(dacSession_t *session, const char *key, unsigned char *value)
{
	return 0;
}
