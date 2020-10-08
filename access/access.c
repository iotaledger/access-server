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

#include "access.h"

#include <stdlib.h>
#include <string.h>

#include "pap_plugin.h"
#include "pep.h"
#include "pep_plugin.h"
#include "pip.h"
#include "timer.h"

void access_init() {
  pep_init();
  pip_init();
}

void access_start() {
  pip_start();
}

void access_term() {
  pip_term();
  pep_term();
}

int access_register_pep_plugin(plugin_t *plugin) {
  pep_register_plugin(plugin);
}

int access_register_pip_plugin(plugin_t *plugin) {
  pip_register_plugin(plugin);
}

int access_register_pap_plugin(plugin_t *plugin) {
  pap_register_plugin(plugin);
}
