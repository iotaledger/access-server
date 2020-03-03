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
 ****************************************************************************/


#ifndef _RESOLVER_H_
#define _RESOLVER_H_

#define MAX_LED_NUMBER 10
#define CAR_STATE_LOCK 1
#define CAR_STATE_UNLOCK 2
#define CAR_STATE_OPEN_TRUNK 3
#define CAR_STATE_CLOSE_TRUNK 4


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
 * @fn  void Resolver_action02(void)
 *
 * @brief   Indication of car lock
 *
 */
int Resolver_action02();

/**
 * @fn  void Resolver_action01(void)
 *
 * @brief   Indication of car unlock
 *
 */
int Resolver_action01();

/**
 * @fn  void Resolver_action03(void)
 *
 * @brief   Indication of trunk open
 *
 */
int Resolver_action03();


int Resolver_action04();
int Resolver_action05();

/**
 * @fn  void policy_update_indication(void)
 *
 * @brief   Indication of the policy store update
 *
 */
int policy_update_indication();

int get_time(char *buf);

void Resolver_set_relayboard_addr(const char* addr);

void Resolver_init_can01_remote();
void Resolver_init_can01_remote_tcp();
void Resolver_init_canopen01();

#endif //_RESOLVER_H_
