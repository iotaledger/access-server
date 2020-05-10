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
 * Implemntation for ssl based authentication module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

#include "libdac_internal.h"
#include "libdacUtils.h"

#if ((__BYTE_ORDER__) == (__ORDER_BIG_ENDIAN__))

#define htons(A) (A)
#define htonl(A) (A)
#define ntohs(A) (A)
#define ntohl(A) (A)

#elif ((__BYTE_ORDER__) == (__ORDER_LITTLE_ENDIAN__))

#define htons(A) ((((unsigned short)(A) & 0xff00) >> 8) | \
                  (((unsigned short)(A) & 0x00ff) << 8))
#define htonl(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
                   (((unsigned int)(A) & 0x00ff0000) >>  8) | \
                   (((unsigned int)(A) & 0x0000ff00) <<  8) | \
                   (((unsigned int)(A) & 0x000000ff) << 24))
#define ntohs htons
#define ntohl htohl

#else

#error "Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both."

#endif

#if ((__BYTE_ORDER__) == (__ORDER_LITTLE_ENDIAN__))

#endif

void dacDebugBinary(char *name, unsigned char* data, int len)
{
#ifndef NDEBUG
   int i;
   printf("\n%s (%d) = ", name, len);
   for (i = 0; i < len; i++)
   {
      printf("%02X", data[i]);
   }
   printf("\n\n");
#endif
}

unsigned char countOne(unsigned char number)
{

   return (number + 1) ? (number + 1) : 1;
}

void randmem(unsigned char *randomMem, int length)
{

   if (NULL != randomMem)
   {
      if (0 < length)
      {
         int n = 0;
         for (; n < length; n++)
         {
            randomMem[n] = (unsigned char)rand();
         }
      }
   }
}

static int dacInternalWrite(dacSession_t *session, void *data, unsigned short dataLen)
{
   int ret = DAC_ERROR;
   int i = 0;

   while (i < dataLen)
   {
      int len = session->f_write(session->ext, data + i, dataLen - i);
      //TODO: Check return
      if(0 >= len)
      {
         break;
      }

      i += len;
   }

   return ret;
}

static int dacInternalRead(dacSession_t *session, void *data, unsigned short dataLen)
{
   int ret = DAC_ERROR;
   int i = 0;

   while (i < dataLen)
   {
      int len = session->f_read(session->ext, data + i, dataLen - i);
      //TODO: Check return
      if(0 >= len)
      {
         break;
      }

      i += len;
   }

   return ret;
}

int SendMessagePart(dacSession_t *session, void *data, unsigned short dataLen)
{
   int ret = DAC_ERROR;

   if (NULL != data)
   {
      unsigned short len = htons(dataLen);

      ret = dacInternalWrite(session, &len, sizeof(unsigned short));
      //TODO: Check return

      ret = dacInternalWrite(session, data, dataLen);
      //TODO: Check return
   }

   return ret;
}

int SendMessagePartBN(dacSession_t *session, const BIGNUM *bn)
{
   int ret = DAC_ERROR;
   int len = BN_num_bytes(bn);

   unsigned char *data = malloc(len);
   len = BN_bn2bin(bn, data);

   dacDebugBinary("BN", data, len);

   ret = SendMessagePart(session, data, len);
   //TODO: Check return

   free(data);

   return ret;
}

int ReceiveMessagePart(dacSession_t *session, unsigned char **data, unsigned short *dataLen)
{
   int ret = DAC_ERROR;

   if ((NULL != data) && (NULL != dataLen))
   {
      ret = dacInternalRead(session, dataLen, sizeof(unsigned short));
      //TODO: Check return

      *dataLen = ntohs(*dataLen);

      *data = malloc(*dataLen);

      ret = dacInternalRead(session, *data, *dataLen);
      //TODO: Check return
   }

   return ret;
}

int ComputeHash(dacSession_t *session, unsigned char *md, unsigned char *pKey, int pKey_len, const BIGNUM *e,
      const BIGNUM *pub_key)
{
   SHA256_CTX c;
   int ret = DAC_ERROR;
   int len_bn = 0;
   unsigned char *data = NULL;

   SHA256_Init(&c);

   /* computes H  = hash( Vc || Vs || Ks or Kc || e || f || K ) */

   SHA256_Update(&c, getInternalVc(session), DAC_V_STRING_LEN);

   SHA256_Update(&c, getInternalVs(session), DAC_V_STRING_LEN);

   SHA256_Update(&c, pKey, pKey_len);

   len_bn = BN_num_bytes(e);
   data = malloc(len_bn);
   len_bn = BN_bn2bin(e, data);
   SHA256_Update(&c, data, len_bn);
   free(data);

   len_bn = BN_num_bytes(pub_key);
   data = malloc(len_bn);
   len_bn = BN_bn2bin(pub_key, data);
   SHA256_Update(&c, data, len_bn);
   free(data);

   SHA256_Update(&c, getInternalK(session), getInternalK_len(session));

   SHA256_Final(md, &c);

   OPENSSL_cleanse(&c, sizeof(c));

   return ret;
}

void ComputeHashKey(dacSession_t *session, unsigned char *md, char *end)
{
   SHA256_CTX c;

   SHA256_Init(&c);

   /* computes H  = hash( Vc || Vs || Ks or Kc || e || f || K ) */

   SHA256_Update(&c, getInternalK(session), getInternalK_len(session));

   SHA256_Update(&c, getInternalH(session), SHA256_DIGEST_LENGTH);

   SHA256_Update(&c, end, strlen(end));

   SHA256_Final(md, &c);

   OPENSSL_cleanse(&c, sizeof(c));
}

void dacGenerateKeys(dacSession_t *session)
{

   ComputeHashKey(session, getInternalIV_S(session), DAC_HASH_A);
   dacDebugBinary("IV_S", getInternalIV_S(session), SHA256_DIGEST_LENGTH);

   ComputeHashKey(session, getInternalIV_C(session), DAC_HASH_B);
   dacDebugBinary("IV_C", getInternalIV_C(session), SHA256_DIGEST_LENGTH);

   ComputeHashKey(session, getInternalEKey_S(session), DAC_HASH_C);
   dacDebugBinary("EKey_S", getInternalEKey_S(session), SHA256_DIGEST_LENGTH);

   ComputeHashKey(session, getInternalEKey_C(session), DAC_HASH_D);
   dacDebugBinary("EKey_C", getInternalEKey_C(session), SHA256_DIGEST_LENGTH);

   ComputeHashKey(session, getInternalIKey_S(session), DAC_HASH_E);
   dacDebugBinary("IKey_S", getInternalIKey_S(session), SHA256_DIGEST_LENGTH);

   ComputeHashKey(session, getInternalIKey_C(session), DAC_HASH_F);
   dacDebugBinary("IKey_C", getInternalIKey_C(session), SHA256_DIGEST_LENGTH);

}

/* Setup packet
 *    bytes       2         N          X        32
 *          ( pack_len || payload || var_pad || mac )
 */
unsigned char *dacSetupOutPacket(const void *data, unsigned short dataLen, unsigned short *out_len, int *padding_len)
{
   unsigned char *ret_data = NULL;
   unsigned short data_len = htons(dataLen);
   *padding_len = (DAC_AES_DIGEST_LEN - ((dataLen + DAC_MESSAGE_NUMBER_LEN)%DAC_AES_DIGEST_LEN))%DAC_AES_DIGEST_LEN;

   *out_len = DAC_MESSAGE_NUMBER_LEN + dataLen + *padding_len + HMAC_DIGEST_LENGTH;

   ret_data = malloc(*out_len);

   memcpy(ret_data, &data_len, DAC_MESSAGE_NUMBER_LEN);

   memcpy(ret_data + DAC_MESSAGE_NUMBER_LEN, data, dataLen);

   randmem(ret_data + (DAC_MESSAGE_NUMBER_LEN + dataLen), *padding_len);

   return ret_data;
}

static inline int dacHMACUpdate(HMAC_CTX *hmac_ctx, unsigned char **hmac_data, int *hmac_done, int hmac_todo)
{
   int ret = 0;
   ret = HMAC_Update(hmac_ctx, *hmac_data, hmac_todo);
   *hmac_done += hmac_todo;
   *hmac_data += hmac_todo;

   return ret;
}

/* DAC_ERRORS */int dacInternalSend(dacSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv,
      const unsigned char *data, unsigned short  data_len)
{
   int ret = DAC_ERROR;
   unsigned short setup_len = 0, send_len = 0;
   int padding_len = 0;
   unsigned int hmac_len = 0;
   HMAC_CTX *hmac_ctx = HMAC_CTX_new();

   unsigned char *out_data = dacSetupOutPacket(data, data_len, &setup_len, &padding_len);
   unsigned char *hmac_data = out_data;
   int hmac_done = 0;

   debug("data_len (%d), setup_len (%d), padding_len (%d)", data_len, setup_len, padding_len);

   HMAC_Init_ex(hmac_ctx, hmacKey, SHA256_DIGEST_LENGTH, EVP_sha256(), NULL);

   HMAC_Update(hmac_ctx, &getInternalOutP_Count(session), sizeof(getInternalOutP_Count(session)));

   send_len = htons(setup_len);

   HMAC_Update(hmac_ctx, (unsigned char *)&send_len, sizeof(unsigned short));

   dacDebugBinary("outP", out_data, setup_len - HMAC_DIGEST_LENGTH);

   CRYPTO_cbc128_encrypt(out_data, out_data, setup_len - HMAC_DIGEST_LENGTH, aesKey,
         iv, (block128_f) AES_encrypt);

   dacDebugBinary("outPenc", out_data, setup_len - HMAC_DIGEST_LENGTH);

   dacHMACUpdate(hmac_ctx, &hmac_data, &hmac_done, DAC_AES_DIGEST_LEN);

   if (0 != padding_len)
   {
      dacHMACUpdate(hmac_ctx, &hmac_data, &hmac_done, setup_len - HMAC_DIGEST_LENGTH - hmac_done - DAC_AES_DIGEST_LEN);

      dacHMACUpdate(hmac_ctx, &hmac_data, &hmac_done, DAC_AES_DIGEST_LEN);
   }
   else
   {
      dacHMACUpdate(hmac_ctx, &hmac_data, &hmac_done, setup_len - HMAC_DIGEST_LENGTH - hmac_done);
   }

   HMAC_Final(hmac_ctx, hmac_data, &hmac_len);

   debug("setup_len (%d) hmac_len (%d)", setup_len, hmac_len);

   dacDebugBinary("hmac_data", hmac_data, hmac_len);

   ret = dacInternalWrite(session, &send_len, sizeof(unsigned short));
   //TODO: Check return

   ret = dacInternalWrite(session, out_data, setup_len);
   //TODO: Check return

   free(out_data);

   getInternalOutP_Count(session) =  countOne(getInternalOutP_Count(session));

   return ret;
}

unsigned char *dacAlocatePacketLen(unsigned char *digest, unsigned short send_len, unsigned short *payload_len,
      int *data_full, int *padding_len)
{
   unsigned char *ret_data = NULL;

   *data_full = DAC_AES_DIGEST_LEN - DAC_MESSAGE_NUMBER_LEN;
   *payload_len = ntohs(*((unsigned short *)digest));

   if(send_len > *payload_len)
   {
      ret_data = malloc(*payload_len);

      memcpy(ret_data, digest + DAC_MESSAGE_NUMBER_LEN, *data_full);

      *padding_len = (DAC_AES_DIGEST_LEN - ((*payload_len + DAC_MESSAGE_NUMBER_LEN)%DAC_AES_DIGEST_LEN))%DAC_AES_DIGEST_LEN;
   }

   return ret_data;
}

/* DAC_ERRORS */int dacInternalReceive(dacSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv,
      unsigned char **data, unsigned short  *data_len)
{
  int ret = DAC_ERROR;
  int read_len = 0;
  unsigned int hmac_len = 0;
  unsigned short send_len = 0;
  unsigned char digest_buffer[HMAC_DIGEST_LENGTH] = { 0, };
  unsigned char hmac_buffer[HMAC_DIGEST_LENGTH] = { 0, };
  int padding_len = 0, data_full = 0, payload_rest = 0;
  HMAC_CTX *hmac_ctx = HMAC_CTX_new();
  unsigned char *out_data = NULL;

  HMAC_Init_ex(hmac_ctx, hmacKey, SHA256_DIGEST_LENGTH, EVP_sha256(), NULL);

  HMAC_Update(hmac_ctx, &getInternalInP_Count(session), sizeof(getInternalInP_Count(session)));

  ret = dacInternalRead(session, &send_len, sizeof(unsigned short));
        //TODO: Check return

  HMAC_Update(hmac_ctx, (unsigned char *)&send_len, sizeof(unsigned short));

  send_len = ntohs(send_len);

  dacInternalRead(session, digest_buffer, DAC_AES_DIGEST_LEN);

  HMAC_Update(hmac_ctx, digest_buffer, DAC_AES_DIGEST_LEN);

  dacDebugBinary("digest_buffer", digest_buffer, DAC_AES_DIGEST_LEN);

  CRYPTO_cbc128_decrypt(digest_buffer, digest_buffer, DAC_AES_DIGEST_LEN, aesKey,
        iv, (block128_f) AES_decrypt);

  dacDebugBinary("digest_buffer", digest_buffer, DAC_AES_DIGEST_LEN);

  out_data = dacAlocatePacketLen(digest_buffer, send_len, data_len, &data_full, &padding_len);

  debug("data_len (%d), padding_len (%d)", *data_len, padding_len);

  if (0 != padding_len)
  {
     payload_rest = DAC_AES_DIGEST_LEN - padding_len;
  }

  read_len = *data_len - data_full - payload_rest;

  debug("read_len (%d), payload_rest (%d)", read_len, payload_rest);

  dacInternalRead(session, out_data + data_full, read_len);

  HMAC_Update(hmac_ctx, out_data + data_full, read_len);

  dacDebugBinary("out_data + data_full", out_data + data_full, read_len);

  CRYPTO_cbc128_decrypt(out_data + data_full, out_data + data_full, read_len, aesKey,
        iv, (block128_f) AES_decrypt);

  dacDebugBinary("out_data + data_full", out_data + data_full, read_len);

  if (0 != padding_len)
  {
     dacInternalRead(session, digest_buffer, DAC_AES_DIGEST_LEN);

     HMAC_Update(hmac_ctx, digest_buffer, DAC_AES_DIGEST_LEN);

     dacDebugBinary("digest_buffer", digest_buffer, DAC_AES_DIGEST_LEN);

     CRYPTO_cbc128_decrypt(digest_buffer, digest_buffer, DAC_AES_DIGEST_LEN, aesKey,
           iv, (block128_f) AES_decrypt);

     dacDebugBinary("digest_buffer", digest_buffer, DAC_AES_DIGEST_LEN);

     memcpy(out_data + *data_len - payload_rest, digest_buffer, payload_rest);


     debug("1 (%c), 2 (%c)", out_data[*data_len - 2], out_data[*data_len - 1]);

  }

  HMAC_Final(hmac_ctx, digest_buffer, &hmac_len);

  dacInternalRead(session, hmac_buffer, HMAC_DIGEST_LENGTH);

  ret = memcmp(hmac_buffer, digest_buffer, HMAC_DIGEST_LENGTH);

  dacDebugBinary("hmac_data", digest_buffer, hmac_len);

  dacDebugBinary("hmac_buffer", hmac_buffer, hmac_len);

  debug("memcmp (%d)", ret);

  getInternalInP_Count(session) = countOne(getInternalInP_Count(session));

  *data = out_data;

  return ret;
}

