/*
 * libdacUtils.h
 *
 *  Created on: Aug 10, 2018
 *      Author: ntt-xain
 */

#ifndef LIBDACUTILS_H_
#define LIBDACUTILS_H_

#include <stdlib.h>
#include <string.h>

//#define NDEBUG

#include "dacdbg.h"

#include "libdac_internal.h"

unsigned char countOne(unsigned char number);

void randmem(unsigned char *randomString, int length);

int SendMessagePart(dacSession_t *session, void *data, unsigned short dataLen);

int SendMessagePartBN(dacSession_t *session, const BIGNUM *bn);

int ReceiveMessagePart(dacSession_t *session, unsigned char **data, unsigned short *dataLen);

int ComputeHash(dacSession_t *session, unsigned char *md,
      unsigned char *pKey, int pKey_len, const BIGNUM *e, const BIGNUM *pub_key);

void ComputeHashKey(dacSession_t *session, unsigned char *md, char *end);

void dacDebugBinary(char *name, unsigned char* data, int len);

void dacGenerateKeys(dacSession_t *session);

/* DAC_ERRORS */int dacInternalSend(dacSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv, const unsigned char *data, unsigned short  data_len);

/* DAC_ERRORS */int dacInternalReceive(dacSession_t *session, const void *hmacKey, AES_KEY *aesKey, unsigned char *iv, unsigned char **data, unsigned short  *data_len);

#endif /* LIBDACUTILS_H_ */
