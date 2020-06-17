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
 ****************************************************************************/
#ifndef ASN_AUTH_HELPER_H
#define ASN_AUTH_HELPER_H

#include "asn_auth.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef enum {
  ADH_NO_ERROR_CLRALLUSR = 9,
  ADH_NO_ERROR_GETALLUSR = 8,
  ADH_NO_ERROR_REGUSR = 7,
  ADH_NO_ERROR_GETUSRID = 6,
  ADH_NO_ERROR_GETUSR = 5,
  ADH_NO_ERROR_GET_DATASET = 4,
  ADH_NO_ERROR_SET_DATASET = 3,
  ADH_NO_ERROR_EN_POL = 2,
  ADH_NO_ERROR_GET_POL_LIST = 1,
  ADH_NO_ERROR = 0,
  ADH_ERROR = -1,
  ADH_ERROR_JSON_NULL = -2,
  ADH_ERROR_CMD_NOT_FND = -3,
  ADH_ERROR_POLID_NOT_FND = -4,
  ADH_ERROR_NEQ_RESOLVE = -5,
  ADH_ERROR_USRID_NOT_FND = -6,
  ADH_ERROR_DATASET_LIST_NOT_FND = -7,
  ADH_ERROR_GETUSR_NOT_FND = -8,
  ADH_ERROR_GETUSRID_NOT_FND = -9,
  ADH_ERROR_REGUSR_NOT_FND = -10
} ADH_error_e;

/**
 * @fn      int asnauth_send_decision(int decision, asn_ctx_t *session,
 * char* response, int size)
 *
 * @brief   Function that send decision to the client that requested action
 *
 * @param   decision      Decision received form pdp
 * @param   session       TCP session handle
 * @param   response      TCP message
 * @param   size          TCP message size
 *
 * @return  0 if it succeeds.
 */
int asnauthhelper_send_decision(int decision, asn_ctx_t *session, char *response, int size);

/**
 * @fn      int asnauthhelper_check_msg_format(const char *request)
 *
 * @brief   Function that checks that format of received request is in right
 *format and form
 *
 * @param   request    Request is json format
 *
 * @return  > 0 if it succeeds, < 0 if it fails.
 * 			 0  if command is "resolve" and request foramt is valid
 * 			 1  if command is "get_policy_list" and request format
 *is valid 2  if command is "enable_policy" and request format is valid 3  if
 *command is "set_dataset" and request format is valid 4  if command is
 *"get_dataset" and request format is valid 5  if command is "get_user" and
 *request format is valid 6  if command is "get_auth_user_id" and request
 *format is valid 7  if command is "register_user" and request format is valid
 * 			 8  if command is "get_all_users" and request format is
 *valid 9  if command is "clear_all_users" and request format is valid
 * 			-2 	if request JSON is NULL
 * 			-3 	if "cmd" string is not found
 * 			-4 	if "policy_id" string is not found
 * 			-5 	if command is not equal to "resolve"
 * 			-6 	if "user_id" string is not found
 * 			-7  if "dataset_list" string is not found
 *			-8  if "get_user" string is not found
 *			-9  if "get_auth_user_id" string is not found
 *			-10 if "register_user" string is not found
 */
int asnauthhelper_check_msg_format(const char *request);

#endif
