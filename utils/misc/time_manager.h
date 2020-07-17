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
 * \project IOTA Access
 * \file time_manager.h
 * \brief
 * Implementation of time manager
 *
 * @Author Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 09.10.2018. Initial version.
 * 15.10.2018 minor changes ( change type uint32 -> unsigned long)
 ****************************************************************************/

#ifndef _TIMEMANAGER_H
#define _TIMEMANAGER_H

/**
 * @fn  timemanager_get_time_string(void)
 *
 * @brief convert epoch time to human readable time format
 * @param buf - pointer to array containing human readable time
 * @param size - size of the buf
 */
void timemanager_get_time_string(char *buf, int size);

/**
 * @fn  timemanager_get_time_epoch()
 *
 * @brief return epoch time
 *
 */
unsigned long timemanager_get_time_epoch();

#endif /* _TIMEMANAGER_H_ */
