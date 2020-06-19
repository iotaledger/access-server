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

/****************************************************************************
 * \project IOTA Access
 * \file gpio_interface.h
 * \brief
 * Raspberry Pi GPIO interface to be used by pip_plugin
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
#ifndef __GPIO_INTERFACE_H__
#define __GPIO_INTERFACE_H__

int gpio_interface_read(int idx);

#endif