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
 * \file relay_interface_test.c
 * \brief
 * Test example for relay_interface module
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
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    printf(
        "usage: %s <command> <relay_idx>\n"
        " - command: on, off, toggle, pulse\n"
        " - relay_idx: relay index\n",
        argv[0]);
    return -1;
  }

  if (strncmp(argv[1], "on", strlen("on")) == 0) {
    relayinterface_on(atoi(argv[2]));
  } else if (strncmp(argv[1], "off", strlen("off")) == 0) {
    relayinterface_off(atoi(argv[2]));
  } else if (strncmp(argv[1], "toggle", strlen("toggle")) == 0) {
    relayinterface_toggle(atoi(argv[2]));
  } else if (strncmp(argv[1], "pulse", strlen("pulse")) == 0) {
    relayinterface_pulse(atoi(argv[2]));
  } else {
    printf("unrecognized command: '%s'\n", argv[1]);
    printf(
        "usage: %s <command> <relay_idx>\n"
        " - command: on, off, toggle, pulse\n"
        " - relay_idx: relay index\n",
        argv[0]);
  }

  return 0;
}
