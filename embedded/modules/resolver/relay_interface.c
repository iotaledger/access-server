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

#include <wiringPi.h>
#include <piFace.h>


// piface stuff
#define PIFACE_BASE  64
#define PIFACE_BASE  64
#define PIFACE_LED   (PIFACE_BASE)

static int pifacerelayplus_initialized = 0;
static void pifacerelayplus_init_maybe()
{
    if (pifacerelayplus_initialized == 0)
    {
        pifacerelayplus_initialized = 1;
        wiringPiSetupSys();
        piFaceSetup(PIFACE_BASE);
    }
}

static int toggle_state[4] = {LOW, LOW, LOW, LOW};

void RelayInterface_init()
{
    pifacerelayplus_init_maybe();
}

void RelayInterface_on(int idx)
{
    printf("RELAY ON %d\n", idx);
    pifacerelayplus_init_maybe();
    toggle_state[idx] = HIGH;
    digitalWrite(PIFACE_LED + idx, HIGH);
}

void RelayInterface_off(int idx)
{
    printf("RELAY OFF %d\n", idx);
    pifacerelayplus_init_maybe();
    toggle_state[idx] = LOW;
    digitalWrite(PIFACE_LED + idx, LOW);
}

void RelayInterface_toggle(int idx)
{
    printf("RELAY TOGGLE %d\n", idx);
    pifacerelayplus_init_maybe();
    if (toggle_state[idx] == LOW)
    {
        toggle_state[idx] = HIGH;
        digitalWrite(PIFACE_LED + idx, HIGH);
    }
    else if (toggle_state[idx] == HIGH)
    {
        toggle_state[idx] = LOW;
        digitalWrite(PIFACE_LED + idx, LOW);
    }
}

void RelayInterface_pulse(int idx)
{
    printf("RELAY PULSE %d\n", idx);
    pifacerelayplus_init_maybe();
    digitalWrite(PIFACE_LED + idx, HIGH);
    delay(500);
    digitalWrite(PIFACE_LED + idx, LOW);
}
