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
 * \file asn_auth_helper.c
 * \brief
 * Implementation of helper functions for pep module
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 02.10.2018. Initial version.
 * 21.02.2020. Added obligations handling
 * 28.02.2020. Added data sharing through action functionality
 ****************************************************************************/

#include "asn_auth_helper.h"
#include <string.h>
#include "json_helper.h"

#define dlog_printf printf

int asnauthhelper_send_decision(int decision, asn_ctx_t *session, char *response, int size) {
  return asnauth_send(session, response, size);
}

int asnauthhelper_check_msg_format(const char *request) {
  if (request == NULL) {
    return ADH_ERROR_JSON_NULL;
  }

  if (jsonhelper_get_value(request, 0, "cmd") == -1) {
    return ADH_ERROR_CMD_NOT_FND;
  }

  int cmd = jsonhelper_get_value(request, 0, "cmd");

  if (memcmp(request + jsonhelper_get_token_start(cmd), "resolve", strlen("resolve")) == 0) {
    if (jsonhelper_get_value(request, 0, "policy_id") == -1) {
      // error invalid request
      return ADH_ERROR_POLID_NOT_FND;
    } else {
      return ADH_NO_ERROR;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "get_policy_list", strlen("get_policy_list")) == 0) {
    if (jsonhelper_get_value(request, 0, "user_id") == -1) {
      // error invalid request
      return ADH_ERROR_USRID_NOT_FND;
    } else {
      return ADH_NO_ERROR_GET_POL_LIST;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "enable_policy", strlen("enable_policy")) == 0) {
    if (jsonhelper_get_value(request, 0, "policy_id") == -1) {
      // error invalid request
      return ADH_ERROR_POLID_NOT_FND;
    } else {
      return ADH_NO_ERROR_EN_POL;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "set_dataset", strlen("set_dataset")) == 0) {
    if (jsonhelper_get_value(request, 0, "dataset_list") == -1) {
      return ADH_ERROR_DATASET_LIST_NOT_FND;
    } else {
      return ADH_NO_ERROR_SET_DATASET;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "get_dataset", strlen("get_dataset")) == 0) {
    return ADH_NO_ERROR_GET_DATASET;
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "get_user", strlen("get_user")) == 0) {
    if (jsonhelper_get_value(request, 0, "username") == -1) {
      return ADH_ERROR_GETUSR_NOT_FND;
    } else {
      return ADH_NO_ERROR_GETUSR;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "get_auth_user_id", strlen("get_auth_user_id")) == 0) {
    if (jsonhelper_get_value(request, 0, "username") == -1) {
      return ADH_ERROR_GETUSRID_NOT_FND;
    } else {
      return ADH_NO_ERROR_GETUSRID;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "register_user", strlen("register_user")) == 0) {
    if (jsonhelper_get_value(request, 0, "user") == -1) {
      return ADH_ERROR_REGUSR_NOT_FND;
    } else {
      return ADH_NO_ERROR_REGUSR;
    }
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "get_all_users", strlen("get_all_users")) == 0) {
    return ADH_NO_ERROR_GETALLUSR;
  } else if (memcmp(request + jsonhelper_get_token_start(cmd), "clear_all_users", strlen("clear_all_users")) == 0) {
    return ADH_NO_ERROR_CLRALLUSR;
  } else {
    return ADH_ERROR_NEQ_RESOLVE;
  }
}
