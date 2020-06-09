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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#include "dataset.h"

#include "json_interface.h"
#include "config_manager.h"

#include "network.h"

#include <time.h>
#include "globals_declarations.h"
#include "timer.h"

#include "access.h"

int g_task_sleep_time;

#define Dlog_printf printf

static volatile int running = 1;
static void signal_handler(int _) { running = 0; }

static Network_actor_ctx_id network_actor_context = 0;

int main(int argc, char** argv)
{
    Dataset_state_t *vdstate;

    signal(SIGINT, signal_handler);
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    ConfigManager_init("config.ini");

    int status = ConfigManager_get_option_int("config", "thread_sleep_period", &g_task_sleep_time);
    if (status != CONFIG_MANAGER_OK) g_task_sleep_time = 1000; // 1 second

    Access_ctx_t access_context = 0;
    Access_init(&access_context);
    Access_get_vdstate(access_context, &vdstate);

    Network_actor_init(vdstate, &network_actor_context);

    Access_start(access_context);
    if (Network_actor_start(network_actor_context) != 0)
    {
        fprintf(stderr, "Error starting Network actor\n");
        running = 0;
    }

    // Wait until process receives SIGINT
    while (running == 1) usleep(g_task_sleep_time);

    Network_actor_stop(&network_actor_context);
    Access_deinit(access_context);

    return 0;
}
