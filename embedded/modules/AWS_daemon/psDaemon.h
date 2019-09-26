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
 * \file psDaemon.h
 * \brief
 * Implementation of the policy store daemon
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 01.10.2018. Initial version.
 ****************************************************************************/

#ifndef _PSDAEMON_H_
#define _PSDAEMON_H_


#define AWS_REQ_GET_LIST_SIZE (256)

#define AWS_REPLY_LIST_SIZE   (1024)

#define AWS_REPLY_POLICY_SIZE (2048)

/* PSD_STAGES */
#define PSD_ERROR            (0)
#define PSD_INIT             (1)
#define PSD_GET_PL           (2)
#define PSD_GET_PL_DONE      (3)
#define PSD_GET_PSS          (4)
#define PSD_GET_PSS_DONE     (5)

#define PSD_STANDARD_TIMEOUT (2)
#define PSD_LONG_TIMEOUT     (40)

int PSDaemon_do_work();
void PSDaemon_set_policy_store_address(const char* address);
void PSDaemon_set_policy_store_port(int port);
void PSDaemon_set_device_id(const char* new_device_id);

#endif /* _PSDAEMON_H_ */
