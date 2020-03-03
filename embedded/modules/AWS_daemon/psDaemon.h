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
