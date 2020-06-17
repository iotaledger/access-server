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

#include "asn_auth.h"
#include "asn_debug.h"
#include "asn_internal.h"

typedef struct asn_struct asn_struct_t;

static asn_struct_t internal;

static int asnauth_init(asn_ctx_t *session, void *ext, int type) {
  int ret = ASN_ERROR;

  if (NULL != session) {
    memset((void *)session, 0, sizeof(asn_ctx_t));

    ASN_GET_INTERNAL(session) = &internal;

    if (NULL != ASN_GET_INTERNAL(session)) {
      memset((void *)ASN_GET_INTERNAL(session), 0, sizeof(asn_struct_t));

      ASN_GET_INTERNAL_TYPE(session) = type;
      session->ext = ext;

      ret = ASN_OK;
    }
  }

  return ret;
}

int asnauth_init_client(asn_ctx_t *session, void *ext) { return asnauth_init(session, ext, ASN_TYPE_CLIENT); }

int asnauth_init_server(asn_ctx_t *session, void *ext) { return asnauth_init(session, ext, ASN_TYPE_SERVER); }

int asnauth_set_option(asn_ctx_t *session, const char *key, unsigned char *value) {
  int ret = ASN_ERROR;

  if (NULL != session) {
    if (ASN_TYPE_SERVER == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_server_set_option(session, key, value);
    } else if (ASN_TYPE_CLIENT == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_client_set_option(session, key, value);
    }
  }

  return ret;
}

int asnauth_authenticate(asn_ctx_t *session) {
  int ret = ASN_ERROR;

  if (NULL != session) {
    if (ASN_TYPE_SERVER == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_server_authenticate(session);
    } else if (ASN_TYPE_CLIENT == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_client_authenticate(session);
    }
  }

  return ret;
}

int asnauth_send(asn_ctx_t *session, const unsigned char *data, unsigned short len) {
  int ret = ASN_ERROR;

  if ((NULL != session) && (NULL != data) && (len > 0)) {
    /* Encrypt and send */
    if (ASN_TYPE_SERVER == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_server_send(session, data, len);
    } else if (ASN_TYPE_CLIENT == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_client_send(session, data, len);
    }
  }

  return ret;
}

int asnauth_receive(asn_ctx_t *session, unsigned char **data, unsigned short *len) {
  int ret = ASN_ERROR;

  if (NULL != session) {
    /* Decrypt and receive */
    if (ASN_TYPE_SERVER == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_server_receive(session, data, len);
    } else if (ASN_TYPE_CLIENT == ASN_GET_INTERNAL_TYPE(session)) {
      ret = asninternal_client_receive(session, data, len);
    }
  }

  return ret;
}

int asnauth_release(asn_ctx_t *session) {
  int ret = ASN_ERROR;

  if (NULL != ASN_GET_INTERNAL(session)) {
    if (ASN_TYPE_SERVER == ASN_GET_INTERNAL_TYPE(session)) {
      asninternal_release_server(session);
    } else if (ASN_TYPE_CLIENT == ASN_GET_INTERNAL_TYPE(session)) {
      asninternal_release_client(session);
    }

    ASN_GET_INTERNAL(session) = NULL;

    ret = ASN_OK;
  }

  if (NULL != session) {
    session = NULL;

    ret = ASN_OK;
  }

  return ret;
}
