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
 * \file network.h
 * \brief
 * Header for network module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.11.2019. Initial version.
 ****************************************************************************/

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "dataset.h"

typedef void* Network_actor_ctx_t;

int Network_actor_init(Dataset_state_t *_vdstate, Network_actor_ctx_t* network_actor_context);
int Network_actor_start(Network_actor_ctx_t network_actor_context);
void Network_actor_stop(Network_actor_ctx_t network_actor_context);

#endif
