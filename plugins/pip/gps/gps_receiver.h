/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \project Decentralized Access Control
 * \file gps_receiver.h
 * \brief
 * Implementation of interface for GPS module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#ifndef _GPS_RECEIVER_H_
#define _GPS_RECEIVER_H_

#include <pthread.h>

typedef enum { GPS_NO_ERROR = 0, GPS_ERROR = -1, GPS_ERROR_START = -2 } gps_error_e;

int gpsreceiver_init(pthread_mutex_t* json_mutex);
int gpsreceiver_start();
int gpsreceiver_end();

#endif
