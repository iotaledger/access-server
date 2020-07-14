/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
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
 * \file gpio_interface.c
 * \brief
 * Raspberry Pi GPIO interface to be used by pip_plugin
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
#include "gpio_interface.h"
#include <stdio.h> 
#include <pigpio.h>

#define LOW 0
#define HIGH 1
#define MIN_IDX 0
#define MAX_IDX 3
#define SLEEP_TIME 0.5

/* BCM Pinout: https://pinout.xyz/ */
uint8_t idx2bcm[] = {4, 17, 27, 22};

static int check_idx(int idx) {
  if (idx < MIN_IDX || idx > MAX_IDX) {
    return -1;
  }
}

int gpio_interface_read(int idx) {
  printf("READ GPIO %d\n", idx);

  if (gpioInitialise() < 0 || check_idx(idx) < 0) {
    fprintf(stderr, "pigpio initialisation failed\n");
    return 1;
  }

  uint8_t bcm = idx2bcm[idx];

  /* Set GPIO modes */
  gpioSetMode(bcm, PI_INPUT);

  int current_state = gpioRead(bcm);

  /* Stop DMA, release resources */
  gpioTerminate();

  return current_state;
}
