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

#include "plugin.h"

int plugin_init(plugin_t *plugin, plugin_cb initializer, void *data) { initializer(plugin, data); }

int plugin_destroy(plugin_t *plugin) { return plugin->destroy(plugin, NULL); }

int plugin_call(plugin_t *plugin, size_t plugin_idx, void *user_data) {
  if (plugin_idx < plugin->callbacks_num && plugin->callbacks[plugin_idx] != NULL) {
    plugin->callbacks[plugin_idx](plugin, user_data);
  }
}
