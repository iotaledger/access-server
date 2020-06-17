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
 * \file modbus.h
 * \brief
 * Modbus RTU interface header file
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.26.2019. Initial version.
 ****************************************************************************/

#ifndef _MODBUS_H_
#define _MODBUS_H_

#include <stdint.h>

#define MODBUS_DEV_NAME_LEN 128

typedef struct {
  char device_name[MODBUS_DEV_NAME_LEN];
  int fd;
} modbus_t;

int modbus_init(modbus_t* modbus, const char* serial_device);

int modbus_read_registers(modbus_t* modbus, int slave_device_address, uint16_t register_address,
                          uint16_t quantity_to_read, int16_t* data);

void modbus_deinit(modbus_t* modbus);

#endif
