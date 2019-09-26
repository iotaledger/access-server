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

#include <stdio.h>
#include <json.h>
#include "jsmn.h"

#include "user_management.h"

static char response[4096];

void main()
{
    UserManagement_init();

    UserManagement_get_all_users(response);
    printf("get_all_users response: '%s'\n", response);

    UserManagement_get_string("jamie", response);
    printf("get jamie response: '%s'\n", response);

    UserManagement_put_string("{ \"username\": \"djura\", \"firstName\": \"Djura\", \"lastName\": \"Djuric\", \"userId\": \"0x824717182474182747282812472724818\" }", response);
    printf("put djura response: '%s'\n", response);

    UserManagement_get_string("djura", response);
    printf("get djura response: '%s'\n", response);

    UserManagement_deinit();
}
