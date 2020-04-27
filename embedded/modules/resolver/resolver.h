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
 * \file resolver.h
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


#ifndef _RESOLVER_H_
#define _RESOLVER_H_

#include <stdlib.h>

#include "vehicle_dataset.h"
#include "vehicle_datasharing_dataset.h"

#define MAX_LED_NUMBER 10
#define MAX_STR_SIZE 256

#define MAX_RESOLVER_ACTIONS 10
#define RESOLVER_ACTION_NAME_SIZE 16

typedef int (*resolver_action_t)(int should_log);

typedef struct {
	char action_names[MAX_RESOLVER_ACTIONS][RESOLVER_ACTION_NAME_SIZE];
    resolver_action_t actions[MAX_RESOLVER_ACTIONS];
    size_t count;
    void (*init_ds_interface_cb)(VehicleDataset_state_t*);
    void (*start_ds_interface_cb)(void);
    void (*stop_ds_interface_cb)(void);
} resolver_plugin_t;

typedef void (*resolver_plugin_initializer_t)(resolver_plugin_t*);

/**
 * @fn  void run_led(void)
 *
 * @brief   run led
 *
 */
int run_led(void);

/**
 * @fn  void set_led_flashing(unsigned char led, unsigned int duration)
 *
 * @brief   led is set flashing for certain duration
 * @param   led    identification of LED in range 0=LED1 to 7=LED8, 8=OUT1 and 9=OUT2 are used for relay driving
 * @param   duration duration of the LED glowing in 50ms
 */
void set_led_flashing(unsigned char led, unsigned int duration);

/**
 * @fn  void set_led_on(unsigned char led, unsigned int duration);
 *
 * @brief   Set led on for certain duration
 * @param   led    identification of LED in range 0=LED1 to 7=LED8, 8=OUT1 and 9=OUT2 are used for relay driving
 * @param   duration duration of the LED glowing in 50ms
 *
 */
void set_led_on(unsigned char led, unsigned int duration);

/**
 * @fn int Resolver_action(const char* action, int should_log, void* arg)
 *
 * @brief Perform action mapped to action string
 * @param action 		name of the action to be performed
 * @param should_log	if action should be logged
 * @param arg			optional parameter for action
 */
int Resolver_action(const char* action, int should_log, void* arg);

/**
 * @fn  int Resolver_start_data_sharing(char*)
 *
 * @brief   Start data sharing
 *
 */
int Resolver_start_data_sharing(const char *action, unsigned long end_time);

/**
 * @fn  int Resolver_action07(void)
 *
 * @brief   Stop data sharing
 *
 */
int Resolver_stop_data_sharing();

/**
 * @fn  void policy_update_indication(void)
 *
 * @brief   Indication of the policy store update
 *
 */
int policy_update_indication();

/**
 * @fn  int get_time(char *buf)
 *
 * @brief   Get current time as string
 *
 */
int get_time(char *buf);

/**
 * @fn  void Resolver_init(resolver_plugin_initializer_t initializer, VehicleDataset_state_t *vdstate)
 *
 * @brief   Initialize Resolver module
 *
 */
void Resolver_init(resolver_plugin_initializer_t initializer, VehicleDataset_state_t *vdstate);


#endif //_RESOLVER_H_
