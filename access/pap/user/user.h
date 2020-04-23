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

#ifndef _USER_MANAGEMENT_H_
#define _USER_MANAGEMENT_H_

int UserManagement_init(const char* bc_hostname, int bc_port, const char* device_id);
void UserManagement_deinit();
void UserManagement_get_all_users(char* response);
void UserManagement_get_string(const char* id, char* response);
void UserManagement_put_string(const char* json_string, char* json_response);
void UserManagement_get_authenteq_id(const char* username, char* json_string);
void UserManagement_clear_all_users(char* response);

#endif
