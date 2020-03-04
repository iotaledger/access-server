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
 * \file timer.c
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
#include <stdint.h>
#include <string.h>
#include <sys/timerfd.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>

#include "timer.h"
#include "Dlog.h"

#define MAX_TIMER_COUNT 1000
#define POLL_TIMEOUT 100

typedef struct timer_node
{
	int					id;
	int					fd;
	time_handler		callback;
	void*				user_data;
	unsigned int		interval;
	timer_mode_t		type;
	struct timer_node*	next;
} timer_node_t;

typedef struct itimerspec itimerspec_t;
typedef struct pollfd pollfd_t;

static void* timer_thread(void* data);
static bool thread_is_running = FALSE;
static int timer_list_size = 0;
static pthread_t thread_id;
static timer_node_t* head = NULL;

int Timer_init(void)
{
	thread_is_running = TRUE;

	if(pthread_create(&thread_id, NULL, timer_thread, NULL))
	{
		Dlog_printf("\n\nERROR[%s]: Thread creation failed\n\n", __FUNCTION__);
		return -1;
	}

	return 1;
}

void Timer_deinit()
{
    while (head)
	{
		Timer_stop(head->id);
	}

	thread_is_running = FALSE;
 
    pthread_cancel(thread_id);
    pthread_join(thread_id, NULL);
}

int Timer_start(unsigned int interval, time_handler handler, timer_mode_t type, void* user_data)
{
	int i = 0;
	timer_node_t* new_node = NULL;
	timer_node_t* temp = NULL;
	itimerspec_t new_value;

	if (user_data == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Bad input parameter\n\n", __FUNCTION__);
		return -1;
	}

	new_node = (timer_node_t*)malloc(sizeof(timer_node_t));

	if (new_node == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Failed to allocate new node\n\n", __FUNCTION__);
		return -1;
	}

	new_node->callback  = handler;
	new_node->user_data = user_data;
	new_node->interval  = interval;
	new_node->type      = type;

	new_node->fd = timerfd_create(CLOCK_REALTIME, 0);

	if (new_node->fd == -1)
	{
		Dlog_printf("\n\nERROR[%s]: Failed to create timerfd\n\n", __FUNCTION__);
		free(new_node);
		return -1;
	}

	new_value.it_value.tv_sec = interval / 1000;
	new_value.it_value.tv_nsec = (interval % 1000)* 1000000;

	if (type == TIMER_PERIODIC)
	{
		new_value.it_interval.tv_sec= interval / 1000;
		new_value.it_interval.tv_nsec = (interval %1000) * 1000000;
	}
	else
	{
		new_value.it_interval.tv_sec= 0;
		new_value.it_interval.tv_nsec = 0;
	}

	timerfd_settime(new_node->fd, 0, &new_value, NULL);

	for(i = 0; i <= timer_list_size; i++)
	{
		bool found = FALSE;
		temp = head;

		while (temp != NULL)
		{
			if (temp->id == i)
			{
				found = TRUE;
				break;
			}

			temp = temp->next;
		}

		if (!found)
		{
			break;
		}
	}

	new_node->id = i;

	/*Inserting the timer node into the list*/
	new_node->next = head;
	head = new_node;
	timer_list_size++;

	return new_node->id;
}

void Timer_stop(int timer_id)
{
	timer_node_t* tmp = NULL;
	timer_node_t* node = head;
	
	while (node)
	{
		if (node->id == timer_id)
		{
			break;
		}
		else
		{
			node = node->next;
		}
	}

	if (node == NULL)
	{
		Dlog_printf("\n\nERROR[%s]: Timer %d is not started\n\n", __FUNCTION__, timer_id);
		return;
	}

	close(node->fd);

	if (node == head)
	{
		head = head->next;
	}
	else
	{
		tmp = head;

		while (tmp && tmp->next != node)
		{
			tmp = tmp->next;
		}

		if (tmp)
		{
			// tmp->next can not be NULL here
			tmp->next = tmp->next->next;
		}
	}

	if (node)
	{
		free(node);
		timer_list_size--;
	}
}

timer_node_t* get_timer_from_fd(int fd)
{
	timer_node_t* tmp = head;

	while(tmp)
	{
		if(tmp->fd == fd)
		{
			return tmp;
		}

		tmp = tmp->next;
	}

	return NULL;
}
 
void* timer_thread(void * data)
{
	int iMaxCount = 0;
	int read_fds = 0;
	int i;
	int s;
	uint64_t exp;
	pollfd_t ufds[MAX_TIMER_COUNT] = {{0}};
	timer_node_t* tmp = NULL;

	while (thread_is_running)
	{
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_testcancel();
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

		iMaxCount = 0;
		tmp = head;

		memset(ufds, 0, sizeof(pollfd_t) * MAX_TIMER_COUNT);
		while (tmp)
		{
			ufds[iMaxCount].fd = tmp->fd;
			ufds[iMaxCount].events = POLLIN;
			iMaxCount++;

			tmp = tmp->next;
		}

		read_fds = poll(ufds, iMaxCount, POLL_TIMEOUT);

		if (read_fds <= 0)
		{
			continue;
		}

		for (i = 0; i < iMaxCount; i++)
		{
			if (ufds[i].revents & POLLIN)
			{
				s = read(ufds[i].fd, &exp, sizeof(uint64_t));

				if (s != sizeof(uint64_t))
				{
					continue;
				}

				tmp = get_timer_from_fd(ufds[i].fd);

				if (tmp && tmp->callback)
				{
					tmp->callback((size_t)tmp, tmp->user_data);
				}
			}
		}
	}

	return NULL;
}
