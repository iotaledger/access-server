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
 * \file pip.c
 * \brief
 * Implementation of Policy Information Point
 *
 * @Author Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 12.10.2018. Initial version.
 * 09.11.2018 Updated to parse request sent from PDP module
 ****************************************************************************/

#include <string.h>
#include <stdio.h>

#include "pip.h"
#include "pip_internal.h"
#include "time_manager.h"

#include "Dlog.h"


unsigned char lockState = 0;
unsigned char trunkState = 0;


static int fetch_data(policy_t *pol, pip_data_id_t data_id, char *data)
{
	int n = 0;
	switch (data_id)
	{
		case PIP_DOOR_ID:
		{
			if(lockState == 0x00)
			{
				n = 4;
				strcpy(data, "true");
			}
			else if(lockState == 0x01)
			{
				n = 5;
				strcpy(data, "false");
			}
			else
			{
				n = PIP_ERROR;
			}
			break;
		}
		case PIP_TRUNK_ID:
		{
			if(trunkState == 0x00)
			{
				n = 4;
				strcpy(data, "true");
			}
			else if(trunkState == 0x01)
			{
				n = 5;
				strcpy(data, "false");
			}
			else
			{
				n = PIP_ERROR;
			}
			break;
		}
		case PIP_TIME_ID:
		{
			char buffer[MAX_NUMBER_OF_DIGITS + 1];
			unsigned long time = getEpochTime();
			n = sprintf(buffer, "%lu", time);
			if(n < 0)
			{
				n = PIP_ERROR;
			}
			else
			{
				strcpy(data, buffer);
			}
			break;
		}
		case PIP_VEHICLE_ID_ID:
		{
			strcpy(data, (char *) (vehicleID ));
			n = strlen(data);
			break;

		}
		case PIP_EXECUTION_NUM_ID:
		{
			sprintf(data, "%d", pol->num_of_executions);
			n = strlen(data);
			break;
		}
		default:
		{
			n = PIP_ERROR;
		}

	}

	return  n;
}



void setLockState(unsigned char state)
{
	if(state == 0)
	{
		lockState = 0;
	}
	else if (state == 1)
	{
		lockState = 1;
	}
	else
	{
		lockState = INVALID_LOCK_STATE;
	}
}

unsigned char getLockState()
{
	return lockState;
}

void setTrunkState(unsigned char state)
{
	if(state == 0)
	{
		trunkState = 0;
	}
	else if (state == 1)
	{
		trunkState = 1;
	}
	else
	{
		trunkState = INVALID_TRUNK_STATE;
	}
}

unsigned char getTrunkState()
{
	return trunkState;
}

int pip_get_data(policy_t *pol, char *request, char *data, int length)
{
	char dot[] = ".";
	char temp[131];
	memcpy(temp,request, length);
	char * ptr  = strtok(temp,dot);

	int ret = -1;

    if(memcmp(ptr,"request",7)==0)
    {
    	ptr = strtok(NULL, dot);
    	if(memcmp(ptr,"subject",7)==0)
    	{

			ptr = strtok(NULL, dot);
			//TODO: change this later
			ret = -2;
			//TODO: implement pip module with support for request for subject
			if(memcmp(ptr, "value", 5 ) == 0)
			{

			}
			else if(memcmp(ptr, "type", 4 ) == 0)
			{

			}
			else
			{

			}

    	}
    	else if( memcmp(ptr,"object",6)==0)
    	{

    		ptr = strtok(NULL, dot);

    		if (memcmp (ptr, "value", 5) == 0)
			{
				ret = fetch_data(pol, PIP_VEHICLE_ID_ID, data);
			}
			else if (memcmp (ptr, "type", 4) == 0)
			{
				strcpy(data, PIP_PUBLIC_ID_TYPE_STR);
				ret = strlen(data);
			}
			else
			{
				ret = -1;
			}
    	}
    	else if(memcmp(ptr, PIP_TIME_TYPE_STR, strlen(PIP_TIME_TYPE_STR))==0)
    	{
    		ptr = strtok(NULL, dot);
    		if (memcmp ( ptr, "value", 5) == 0)
			{
				ret = fetch_data(pol, PIP_TIME_ID, data);
			}
			else if (memcmp ( ptr, "type", 4) == 0)
			{
				strcpy(data, PIP_TIME_TYPE_STR);
				ret = strlen(data);
			}
			else
			{
				Dlog_printf("Invalid request for time\n");
				ret = -1;
			}
    	}
    	else if (memcmp(ptr, PIP_EXECUTION_NUM_STR, strlen(PIP_EXECUTION_NUM_STR)) == 0)
    	{
    		ptr = strtok(NULL, dot);
    		if (memcmp(ptr, "value", 5) == 0)
			{
				ret = fetch_data(pol, PIP_EXECUTION_NUM_ID, data);
			}
			else if (memcmp(ptr, "type", 4) == 0)
			{
				strcpy(data, PIP_EXECUTION_NUM_STR);
				ret = strlen(data);
			}
			else
			{
				Dlog_printf("Invalid request for number of executions\n");
				ret = -1;
			}
    	}
    	else if(strcmp(ptr,"action")==0)
    	{
    		Dlog_printf("It is: %s\n",ptr);
    		ptr = strtok(NULL, dot);
    		//TODO: implement pip module with support for request for action
			if(strcmp(ptr,"value")==0)
			{

			}
			else if(strcmp(ptr,"type")==0)
			{

			}
			else
			{

			}
    	}
    	else
    	{
    		Dlog_printf("invalid request type\n");
    		ret = -1;
    	}
    }
    else
    {
    	ret = -1;
    }

    return ret;

}



