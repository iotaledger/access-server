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
 * \file asn_auth.h
 * \brief
 *    Authentication module API
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 ****************************************************************************/

#ifndef ASN_AUTH_H
#define ASN_AUTH_H

//////////////////////////////////////////
// Include files
//////////////////////////////////////////

#include <stdio.h>

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* ASN_ERRORS */
#define ASN_OK    0
#define ASN_ERROR 1

#define AUTH_SERVER_PORT 9998

//////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
typedef ssize_t f_asn_ext_t(void *, void *, unsigned short);

typedef int f_asn_key_verify(unsigned char *, int);

typedef struct asnStruct asnStruct_t;

typedef struct {
   asnStruct_t *internal;

   void *ext; /* External data structure */

   f_asn_ext_t *f_write;
   f_asn_ext_t *f_read;

   f_asn_key_verify *f_verify;

   int status;
} asnSession_t;

//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////

int asnAuth_init_client(asnSession_t *, void *);
int asnAuth_init_server(asnSession_t *, void *);

int asnAuth_set_option(asnSession_t *, const  char *, unsigned char *);

int asnAuth_authenticate(asnSession_t *);

int asnAuth_send(asnSession_t *, const unsigned char *, unsigned short);

int asnAuth_receive(asnSession_t *, unsigned char **, unsigned short *);

int asnAuth_release(asnSession_t *);
#ifdef __cplusplus
};
#endif
#endif /* ASN_AUTH_H */
