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

#ifndef _CANOPEN_SDO_H_
#define _CANOPEN_SDO_H_

#include <stdint.h>

typedef struct {
    int index;
    char name[64];
    char unit[32];
    float factor;
    float offset;
    int min;
    int max;
} CanopenSdo_SDO_object_t;

#define CANOPEN_SDO_DATA_TYPE_NONE 0
#define CANOPEN_SDO_DATA_TYPE_INT 1
#define CANOPEN_SDO_DATA_TYPE_FLOAT 2

typedef struct {
    double value;
    char unit[32];
    char name[64];
    int type;
} CanopenSdo_parsed_data_t;

#define CANOPEN_SDO_BMS_BAT_VOLTAGE_IDX 0x200136
#define CANOPEN_SDO_ANALOGUE_BRAKE_FULL_VOLTAGE_IDX 0x200318
#define CANOPEN_SDO_ANALOGUE_BRAKE_OFF_VOLTAGE_IDX 0x200318
#define CANOPEN_SDO_CONTROLLER_TEMPERATURE_IDX 0x200404
#define CANOPEN_SDO_VEHICLE_SPEED_IDX 0x200405
#define CANOPEN_SDO_MOTOR_RPM_IDX 0x200408
#define CANOPEN_SDO_MOTOR_SPEED_IDX 0x200409
#define CANOPEN_SDO_BATTERY_VOLTAGE_IDX 0x20040A
#define CANOPEN_SDO_BATTERY_CURRENT_IDX 0x20040B
#define CANOPEN_SDO_BATTERY_SOC_IDX 0x20040C
#define CANOPEN_SDO_THROTTLE_VOLTAGE_IDX 0x20040F
#define CANOPEN_SDO_BRAKE_1_VOLTAGE_IDX 0x200410
#define CANOPEN_SDO_BRAKE_2_VOLTAGE_IDX 0x200411
#define CANOPEN_SDO_BATTERY_POWER_PERCENT_IDX 0x200419
#define CANOPEN_SDO_RAW_BATTERY_VOLTAGE_IDX 0x200424
#define CANOPEN_SDO_WHEEL_RPM_SPEED_SENSOR_BASED_IDX 0x200438
#define CANOPEN_SDO_WHEEL_RPM_MOTOR_BASED_IDX 0x200439
#define CANOPEN_SDO_TRIP_METER_IDX 0x200619
#define CANOPEN_SDO_BRAKE_LIGHT_PWM_DAYLIGHT_IDX 0x200720
#define CANOPEN_SDO_REMOTE_THROTTLE_VOLTAGE_IDX 0x200730
#define CANOPEN_SDO_TPDO0_MAPPING1_IDX 0x1a0001
#define CANOPEN_SDO_TPDO0_MAPPING2_IDX 0x1a0002
#define CANOPEN_SDO_TPDO0_MAPPING3_IDX 0x1a0003
#define CANOPEN_SDO_TPDO0_MAPPING4_IDX 0x1a0004


void CanopenSdo_init();
void CanopenSdo_deinit();

CanopenSdo_SDO_object_t* CanopenSdo_get(int index);
void CanopenSdo_parse(int index, uint64_t data, CanopenSdo_parsed_data_t* parsed_data);

#endif
