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

typedef struct {
    char device_name[128];
    int fd;
} Modbus_t;

int Modbus_init(Modbus_t* modbus, const char* serial_device);

int Modbus_read_registers(Modbus_t* modbus, int slave_device_address, uint16_t register_address, uint16_t quantity_to_read, int16_t *data);

void Modbus_deinit(Modbus_t* modbus);

#endif
