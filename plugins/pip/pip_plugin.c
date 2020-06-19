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
 * \file pip_plugin.c
 * \brief
 * Implementation of PIP Plugin
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pip_plugin.h"
#include <string.h>
#include "datadumper.h"

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static char probe_s[] = "<Data probed>";
static int timerId = -1;
static dataset_state_t *g_dstate = NULL;
static pip_plugin_t pip_plugin_probe_set = {0};

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/

static bool pip_plugin_callback(void *todo) {
    // ToDo: implement this
}


/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
pip_plugin_fn pip_plugin_init(pip_plugin_initializer_t initializer, dataset_state_t *dstate) {
  initializer(&pip_plugin_probe_set);
  g_dstate = dstate;

  return pip_plugin_callback;
}

void pip_plugin_term(pip_plugin_terminator_t terminizer) {
  terminizer(&pip_plugin_probe_set);
  g_dstate = NULL;

  pip_unregister_all_callbacks();
}
