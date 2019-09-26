/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
 * \project Decentralized Access Control
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
 * @fn  time_manager_run()
 *
 * @brief counter generator for time manager
 *
 */
void time_manager_run();

/**
 * @fn  getTime(void)
 *
 * @brief Request to get time using the TIME protocol, RFC-868
 *
 */
void getTime(void);

/**
 * @fn  getStringTime(void)
 *
 * @brief convert epoch time to human readable time format
 * @param buf - pointer to array containing human readable time
 * @param size - size of the buf
 */
void getStringTime(char *buf, int size);

/**
 * @fn  getEpochTime()
 *
 * @brief return epoch time
 *
 */
unsigned long getEpochTime();

#endif /* _TIMEMANAGER_H_ */
