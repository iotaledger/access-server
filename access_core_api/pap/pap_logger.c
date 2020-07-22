/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
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
 * \file pap_logger.c
 * \brief
 * Logger for Policy Administration Point
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 17.07.2020. Initial version.
 ****************************************************************************/

#include "pap_logger.h"

#define PAP_LOGGER_ID "pap"

logger_id_t pap_logger_id;

void logger_init_pap(logger_level_t level) {
  pap_logger_id = logger_helper_enable(PAP_LOGGER_ID, level, true);
  log_info(pap_logger_id, "[%s:%d] enable logger %s.\n", __func__, __LINE__, PAP_LOGGER_ID);
}

void logger_destroy_pap() {
  log_info(pap_logger_id, "[%s:%d] destroy logger %s.\n", __func__, __LINE__, PAP_LOGGER_ID);
  logger_helper_release(pap_logger_id);
}
