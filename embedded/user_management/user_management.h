/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
