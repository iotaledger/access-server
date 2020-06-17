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

#include <stdio.h>

/* ASN_ERRORS */
#define ASN_OK 0
#define ASN_ERROR 1

#define AUTH_SERVER_PORT 9998

#ifdef __cplusplus
extern "C" {
#endif

typedef ssize_t f_asn_ext_t(void *, void *, unsigned short);

typedef int f_asn_key_verify(unsigned char *, int);

typedef struct asn_struct asn_struct_t;

typedef struct {
  asn_struct_t *internal;

  void *ext; /* External data structure */

  f_asn_ext_t *f_write;
  f_asn_ext_t *f_read;

  f_asn_key_verify *f_verify;

  int status;
} asn_ctx_t;

int asnauth_init_client(asn_ctx_t *, void *);
int asnauth_init_server(asn_ctx_t *, void *);

int asnauth_set_option(asn_ctx_t *, const char *, unsigned char *);

int asnauth_authenticate(asn_ctx_t *);

int asnauth_send(asn_ctx_t *, const unsigned char *, unsigned short);

int asnauth_receive(asn_ctx_t *, unsigned char **, unsigned short *);

int asnauth_release(asn_ctx_t *);
#ifdef __cplusplus
};
#endif
#endif /* ASN_AUTH_H */
