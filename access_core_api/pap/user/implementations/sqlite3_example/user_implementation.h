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
 */

#ifndef _USER_IMPLEMENTATION_H_
#define _USER_IMPLEMENTATION_H_

int userimplementation_init_cb();
void userimplementation_deinit_cb();
void userimplementation_get_all_cb(char* response);
void userimplementation_get_obj_cb(const char* username, char* user_object);
void userimplementation_put_obj_cb(const char* user_object, char* json_response);
void userimplementation_get_user_id_cb(const char* username, char* json_string);
void userimplementation_clear_all_cb(char* response);

#endif
