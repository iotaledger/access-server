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
 * \file libdacUtils.h
 * \brief
 * Header file for ssl based authentication module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

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
