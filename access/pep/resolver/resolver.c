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
 * \file resolver.c
 * \brief
 * Implementation of Resolver for Raspberry Pi 3B+ board
 *
 * @Author Vladimir Vojnovic
 *
 * \notes
 *
 * \history
 * 03.10.2018. Initial version.
 * 28.02.2020. Added data sharing through action functionality
 ****************************************************************************/

#include <unistd.h>
#include <strings.h>
#include <string.h>

#include "resolver.h"
#include "Dlog.h"

#include <time.h>
#include <sys/time.h>

#include "can_receiver.h"
#include "canopen_receiver.h"
#include "json_interface.h"
#include "timer.h"
#include "time_manager.h"

#define BOARD_INDICATION
#define CAR_RELAY
#define DATASET_NUM_SIZE 2
#define DATASET_NUM_POSITION 9

unsigned int flashing_counter[MAX_LED_NUMBER]={0};
unsigned int on_counter[MAX_LED_NUMBER]={0};

static int timerId = -1;

static VehicleDataset_state_t *g_vdstate = NULL;

int car_status=0;
void set_led_flashing(unsigned char led, unsigned int duration)
{
    flashing_counter[led]=duration;
}

void set_led_on(unsigned char led, unsigned int duration)
{
    on_counter[led]=duration;
}

int get_time(char *buf)
{
    time_t     now;
    struct tm  ts;
    //char       buf[80];

    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, 80, "%H:%M:%S", &ts);
}


char action_s[] = "<Action performed>";


static resolver_plugin_t resolver_action_set = {0};

void Resolver_init(resolver_plugin_initializer_t initializer, VehicleDataset_state_t *vdstate)
{
    initializer(&resolver_action_set);
    g_vdstate = vdstate;
}

int run_led()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s run led\n", buf, action_s);

    return 0;
}

void timer_handler(size_t timer_id, void * user_data)
{
    Resolver_stop_data_sharing();
}

int Resolver_action(const char* action, int should_log, void* arg)
{
    char buf[80];
    int retval = -1;

    if (0 == memcmp(action, "start_ds_", strlen("start_ds_") - 1))
    {
        retval = Resolver_start_data_sharing(action, *((unsigned long*)arg));
    }
    else if (0 == memcmp(action, "stop_ds", strlen("stop_ds")))
    {
        retval = Resolver_stop_data_sharing();
    }
    else
    {
        for (int i = 0; i < resolver_action_set.count; i++)
        {
            if (memcmp(action, resolver_action_set.action_names[i], strlen(resolver_action_set.action_names[i])) == 0)
            {
                get_time(buf);
                Dlog_printf("%s %s\t%s\n", buf, action, action_s);
                retval = resolver_action_set.actions[i](should_log);
                break;
            }
        }
    }
    return retval;
}

int Resolver_start_data_sharing(const char *action, unsigned long end_time)
{
    if (action == NULL)
    {
        Dlog_printf("\nERROR[%s] - Wrong input parameter\n", __FUNCTION__);
        return -1;
    }

    // Initialize interface
    resolver_action_set.init_ds_interface_cb(g_vdstate);

    // Chose dataset
    char* dataset;
    char dataset_num[DATASET_NUM_SIZE];

    memcpy(dataset_num, &action[DATASET_NUM_POSITION], DATASET_NUM_SIZE);

    switch(atoi(dataset_num))
    {
        case 1:
            dataset = VehicleDataset01_options;
            break;
        case 2:
            dataset = VehicleDataset02_options;
            break;
        case 3:
            dataset = VehicleDataset03_options;
            break;
        case 4:
            dataset = VehicleDataset04_options;
            break;
        case 5:
            dataset = VehicleDataset05_options;
            break;
        case 6:
            dataset = VehicleDataset06_options;
            break;
        case 7:
            dataset = VehicleDataset07_options;
            break;
        case 8:
            dataset = VehicleDataset08_options;
            break;
        case 9:
            dataset = VehicleDataset09_options;
            break;
        case 10:
            dataset = VehicleDataset10_options;
            break;
        case 11:
            dataset = VehicleDataset11_options;
            break;
        case 12:
            dataset = VehicleDataset12_options;
            break;
        case 13:
            dataset = VehicleDataset13_options;
            break;
        case 14:
            dataset = VehicleDataset14_options;
            break;
        case 15:
            dataset = VehicleDataset15_options;
            break;
        case 16:
            dataset = VehicleDataset16_options;
            break;
        default:
            dataset = NULL;
            break;
    }

    // Tokenize dataset
    char* tok = NULL;

    tok = strtok(dataset, "|");

    while (tok != NULL)
    {
        for (int i = 0; i < g_vdstate->options_count; i++)
        {
            // Set options that needs to ne acquried
            if (strncmp(g_vdstate->tokens[i].name, tok, strlen(tok) < strlen(g_vdstate->tokens[i].name) ? strlen(tok) : strlen(g_vdstate->tokens[i].name)) == 0)
            {
                g_vdstate->tokens[i].val = 1;
            }
        }

        tok = strtok(NULL, "|");
    }

    unsigned char *dataset_uint8 = (unsigned char*)g_vdstate->dataset;

    for (int i = 0; i < g_vdstate->options_count; i ++)
    {
        dataset_uint8[i] = g_vdstate->tokens[i].val;
    }

    timerId = Timer_start(end_time - getEpochTime(), (time_handler)timer_handler, TIMER_SINGLE_SHOT, NULL);
    resolver_action_set.start_ds_interface_cb();

    return 0;
}

int Resolver_stop_data_sharing()
{
    Timer_stop(timerId);
    resolver_action_set.stop_ds_interface_cb();

    return 0;
}

int policy_update_indication()
{
    //Dlog_printf("policy_update_indication\n");

    return 0;
}
