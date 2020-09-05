/*
 * This file is part of the IOTA Access distribution
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

#include "request_listener_logger.h"

#define REQUEST_LISTENER_LOGGER_ID "req_list"

logger_id_t request_listener_logger_id;

void logger_init_request_listener(logger_level_t level) {
  request_listener_logger_id = logger_helper_enable(REQUEST_LISTENER_LOGGER_ID, level, true);
  log_info(request_listener_logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, REQUEST_LISTENER_LOGGER_ID);
}

void logger_destroy_request_listener() {
  log_info(request_listener_logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, REQUEST_LISTENER_LOGGER_ID);
  logger_helper_release(request_listener_logger_id);
}
