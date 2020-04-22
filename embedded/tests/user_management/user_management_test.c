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
