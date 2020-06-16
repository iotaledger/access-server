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
 * Implementation of JSON parser using jsmn
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 11.10.2018. Initial version.
 ****************************************************************************/
#include "jsmn.h"

#define JSON_ERROR (-1)
#define bool _Bool

int json_parser_init(const char *policy);
//void json_parser_init();

//int json_get_value(const char *json, jsmntok_t *tok, const char *s);
int json_get_value(const char *json, int position, const char *s);


int get_start_of_token(int token_id);

int get_end_of_token(int token_id);

int get_size_of_token(int token_id);

int get_array_size(int token_id);

int get_array_member(int token_id, int member_index);

int get_attribute_from_array(int token_id, int index);

int has_token(const char *policy, const char *value, int size);

int get_token_value_with_type(const char *policy, const char *type, int type_size);


int get_token_type(int token_id);

int get_action(char *action, char* policy, int number_of_tokens);

jsmntok_t get_token_at(int i);

int json_get_token_index(const char *json, const char *s);

int json_get_token_index_from_pos(const char *json, int pos, const char *s);

int get_token_num(void);

