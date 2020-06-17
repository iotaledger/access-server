/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2020 XAIN AG.
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
 * \file timer.h
 * \brief
 * Timer module
 *
 * @Author Strahinja Golic
 *
 * \notes
 *
 * \history
 * 05.03.2020. Initial version.
 ****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdlib.h>

#ifndef bool
#define bool _Bool
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef enum {
  TIMER_SINGLE_SHOT = 0,  // Periodic Timer
  TIMER_PERIODIC          // Single Shot Timer
} timer_mode_t;

typedef void (*time_handler)(int timer_id, void *user_data);

int timer_init(void);
void timer_deinit(void);
int timer_start(unsigned int interval, time_handler handler, timer_mode_t type, void *user_data);
void timer_stop(int timer_id);
#endif
