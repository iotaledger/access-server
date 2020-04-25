/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \project IOTA Access
 * \file relay_interface.c
 * \brief
 * Relay board interface to be used by resolver
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/

#include "relay_interface.h"

#include <stdio.h>

#include <pigpio.h>

#define	LOW	0
#define	HIGH	1

/* BCM Pinout: https://pinout.xyz/ */
uint8_t idx2bcm[] = {4, 17, 27, 22};

static int check_idx(int idx){
    if (idx < 0 || idx > 3){
        return -1;
    }
}

int RelayInterface_on(int idx)
{
    printf("RELAY ON %d\n", idx);

    if (gpioInitialise() < 0 || check_idx(idx) < 0)
    {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }

    uint8_t bcm = idx2bcm[idx];

    /* Set GPIO mode */
    gpioSetMode(bcm, PI_OUTPUT);

    /* on */
    gpioWrite(bcm, HIGH);

    /* Stop DMA, release resources */
    gpioTerminate();

    return 0;
}

int RelayInterface_off(int idx)
{
    printf("RELAY OFF %d\n", idx);
 
    if (gpioInitialise() < 0 || check_idx(idx) < 0)
    {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }

    uint8_t bcm = idx2bcm[idx];

    /* Set GPIO modes */
    gpioSetMode(bcm, PI_OUTPUT);

    /* off */
    gpioWrite(bcm, LOW);

    /* Stop DMA, release resources */
    gpioTerminate();

    return 0;
}

int RelayInterface_toggle(int idx)
{
    printf("RELAY TOGGLE %d\n", idx);

    if (gpioInitialise() < 0 || check_idx(idx) < 0)
    {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }

    uint8_t bcm = idx2bcm[idx];

    /* Set GPIO modes */
    gpioSetMode(bcm, PI_OUTPUT);

    int current_state = gpioRead(bcm);
    if (current_state == LOW){
        gpioWrite(bcm, HIGH); /* on */
    } else if (current_state == HIGH){
        gpioWrite(bcm, LOW); /* off */
    }

    /* Stop DMA, release resources */
    gpioTerminate();

    return 0;
}

int RelayInterface_pulse(int idx)
{
    printf("RELAY PULSE %d\n", idx);

    if (gpioInitialise() < 0 || check_idx(idx) < 0)
    {
        fprintf(stderr, "pigpio initialisation failed\n");
        return 1;
    }

    uint8_t bcm = idx2bcm[idx];

    /* Set GPIO modes */
    gpioSetMode(bcm, PI_OUTPUT);

    gpioWrite(bcm, HIGH); /* on */
    time_sleep(0.5);
    gpioWrite(bcm, LOW); /* off */

    /* Stop DMA, release resources */
    gpioTerminate();

    return 0;
}
