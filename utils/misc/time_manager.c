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
 * \file time_manager.c
 * \brief
 * Implementation of time manager
 *
 * @Author Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 09.10.2018. Initial version.
 ****************************************************************************/

#include <time.h>

#include "time_manager.h"

#define TIME_ZONE_SHIFT_S 7200  // UTC + 2h

void timemanager_get_time_string(char *buf, int size) {
  time_t l_time = (time_t)(time(NULL) + TIME_ZONE_SHIFT_S);
  struct tm tm;

  tm = *localtime(&l_time);

  strftime(buf, size, "%a %Y-%m-%d %H:%M:%S ", &tm);
}

unsigned long timemanager_get_time_epoch() { return time(NULL); }
