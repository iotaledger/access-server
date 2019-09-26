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

#include <stdio.h>
#include <time.h>

#include "Dlog.h"
#include "time_manager.h"

void getStringTime(char *buf, int size)
{
	time_t l_time = (time_t) (time(NULL) + 7200);
	struct tm tm;

	tm = *localtime(&l_time);

	strftime(buf, size, "%a %Y-%m-%d %H:%M:%S ", &tm);
}

unsigned long getEpochTime()
{
	return time(NULL);
}
