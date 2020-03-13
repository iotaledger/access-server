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
 * \file authDacHelper.c
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

#include "libauthdac.h"
#include "parson.h"

#define TRUE 1
#define FALSE 0

/**
 * @fn      int sendDecision(int decision, dacSession_t *session)
 *
 * @brief   Function that send decision to the client that requested action
 *
 * @param   decision      Decision received form pdp
 * @param   session       TCP session handle
 *
 * @return  0 if it succeeds.
 */
int sendDecision(int decision, dacSession_t *session);
int sendDecision_new(int decision, dacSession_t *session, char* response, int size);
/**
 * @fn      int checkMsgFormat(JSON_Value *request_json)
 *
 * @brief   Function that checks that format of received request is in right format and form
 *
 * @param   request_json    Request is json format
 *
 * @return  0 if it succeeds, < 0 if it failes.
 * 			-2 if request JSON is NULL
 * 			-3 if "cmd" string is not found
 * 			-4 if "policy_id" string is not found
 * 			-5 if command is not equal to "resolve"
 */
int checkMsgFormat(JSON_Value *request_json);

/**
 * @fn      int ledControl(int decision, unsigned char* polID)
 *
 * @brief   Function that controls led lights based on decision received from pdp
 *
 * @param   decision        Decision from pdp
 * @param   obligation      Obligation from policy
 * @param   action          Action from policy
 * @param   start_time      Time when action starts
 * @param   end_time        Time when action ends
 *
 * @return  0 if it succeeds.
 */
int ledControl(int decision, char *obligation, char *action, unsigned long start_time, unsigned long end_time);

/**
 * @fn      int checkMsgFormat_new(const char *request)
 *
 * @brief   Function that checks that format of received request is in right format and form
 *
 * @param   request    Request is json format
 *
 * @return  > 0 if it succeeds, < 0 if it fails.
 * 			 0  if command is "resolve" and request foramt is valid
 * 			 1  if command is "get_policy_list" and request format is valid
 * 			 2  if command is "enable_policy" and request format is valid
 * 			 3  if command is "set_dataset" and request format is valid
 * 			 4  if command is "get_dataset" and request format is valid
 * 			-2 	if request JSON is NULL
 * 			-3 	if "cmd" string is not found
 * 			-4 	if "policy_id" string is not found
 * 			-5 	if command is not equal to "resolve"
 * 			-6 	if "user_id" string is not found
 * 			-7  if "dataset_list" string is not found
 */
int checkMsgFormat_new(const char *request);
