/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
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
 *
 * \file json_parser.h
 * \brief
 * JSON helper wrappers around jsmn JSON parser library
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 11.10.2018. Initial version.
 ****************************************************************************/
#define JSMN_HEADER
#include "jsmn.h"

#define JSON_ERROR (-1)
#define bool _Bool

int jsonhelper_parser_init(const char *policy);

int jsonhelper_get_value(const char *json, int position, const char *s);

int jsonhelper_get_token_start(int token_id);

int jsonhelper_get_token_end(int token_id);

int jsonhelper_token_size(int token_id);

int jsonhelper_array_size(int token_id);

int jsonhelper_get_array_element(int token_id, int member_index);

int jsonhelper_get_attribute_from_array(int token_id, int index);

int jsonhelper_has_token(const char *policy, const char *value, int size);

int jsonhelper_get_value_with_type(const char *policy, const char *type, int type_size);

int jsonhelper_get_token_type(int token_id);

// @TODO: should remove this, as action has nothing to do with json
int jsonhelper_get_action(char *action, char *policy, int number_of_tokens);

jsmntok_t jsonhelper_get_token_at(int i);

int jsonhelper_get_token_index(const char *json, const char *s);

int jsonhelper_get_token_index_from_pos(const char *json, int pos, const char *s);

int jsonhelper_get_token_num(void);
