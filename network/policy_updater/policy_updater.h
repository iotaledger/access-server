/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
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

#ifndef _POLICY_UPDATER_H_
#define _POLICY_UPDATER_H_

void PolicyUpdater_init();

void PolicyUpdater_get_policy(char *policy_id, char *policy_buff);

unsigned int PolicyUpdater_get_policy_list(const char *policy_store_version, const char* device_id, char *policy_list, int *policy_list_len, int *new_policy_list_flag);

#endif /* _POLICY_UPDATER_H_ */
