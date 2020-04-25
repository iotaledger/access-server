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

#include "canopen_sdo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static CanopenSdo_SDO_object_t sdo_db[] = {
    {CANOPEN_SDO_BMS_BAT_VOLTAGE_IDX, "bms bat voltage", "", 32, 0.0, -32768, 32767},
    {CANOPEN_SDO_ANALOGUE_BRAKE_FULL_VOLTAGE_IDX, "analogue brake full voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_ANALOGUE_BRAKE_OFF_VOLTAGE_IDX, "analogue brake off voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_CONTROLLER_TEMPERATURE_IDX, "controller temperature", "deg C", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_VEHICLE_SPEED_IDX, "vehicle speed", "km/hr", 256, 0.0, -32768, 32767},
    {CANOPEN_SDO_MOTOR_RPM_IDX, "motor rpm", "RPM", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_MOTOR_SPEED_IDX, "motor speed", "% or rated rpm", 40.96, 0.0, -32768, 32767},
    {CANOPEN_SDO_BATTERY_VOLTAGE_IDX, "battery voltage", "Volts", 32, 0.0, -32768, 32767},
    {CANOPEN_SDO_BATTERY_CURRENT_IDX, "battery current", "Amps", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_BATTERY_SOC_IDX, "battery state of charge", "%", 1, 0.0, -32768, 100},
    {CANOPEN_SDO_THROTTLE_VOLTAGE_IDX, "throttle voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_BRAKE_1_VOLTAGE_IDX, "brake 1 voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_BRAKE_2_VOLTAGE_IDX, "brake 2 voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_BATTERY_POWER_PERCENT_IDX, "battery power percent", "% of rated power", 1, 0.0, -32768, 100},
    {CANOPEN_SDO_RAW_BATTERY_VOLTAGE_IDX, "raw battery voltage", "pu V", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_WHEEL_RPM_SPEED_SENSOR_BASED_IDX, "wheel rpm (speed sensor based)", "RPM", 16, 0.0, -32768, 32767},
    {CANOPEN_SDO_WHEEL_RPM_MOTOR_BASED_IDX, "wheel rpm (motor based)", "RPM", 16, 0.0, -32768, 32767},
    {CANOPEN_SDO_TRIP_METER_IDX, "trip meter", "km", 100, 0.0, -32768, 32767},
    {CANOPEN_SDO_REMOTE_THROTTLE_VOLTAGE_IDX, "remote throttle voltage", "Volts", 4096, 0.0, -32768, 32767},
    {CANOPEN_SDO_TPDO0_MAPPING1_IDX, "TPDO0 map1", "", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_TPDO0_MAPPING2_IDX, "TPDO0 map2", "", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_TPDO0_MAPPING3_IDX, "TPDO0 map3", "", 1, 0.0, -32768, 32767},
    {CANOPEN_SDO_TPDO0_MAPPING4_IDX, "TPDO0 map4", "", 1, 0.0, -32768, 32767},
};

void CanopenSdo_init(){}
void CanopenSdo_deinit(){}

void CanopenSdo_parse(int index, uint64_t data, CanopenSdo_parsed_data_t* parsed_data)
{
    CanopenSdo_SDO_object_t* sdobj = CanopenSdo_get(index);
    if (sdobj != NULL) {
        strncpy(parsed_data->unit, sdobj->unit, sizeof(parsed_data->unit) / sizeof(parsed_data->unit[0]));
        strncpy(parsed_data->name, sdobj->name, sizeof(parsed_data->name) / sizeof(parsed_data->name[0]));
        parsed_data->value = (float)data / sdobj->factor + sdobj->offset;
        if (parsed_data->value > sdobj->max) parsed_data->value = sdobj->max;
        if (parsed_data->value < sdobj->min) parsed_data->value = sdobj->min;
    }
    switch (index) {
        case CANOPEN_SDO_CONTROLLER_TEMPERATURE_IDX:
        break;
        case CANOPEN_SDO_VEHICLE_SPEED_IDX:
        break;
        case CANOPEN_SDO_MOTOR_RPM_IDX:
        break;
        case CANOPEN_SDO_MOTOR_SPEED_IDX:
        break;
        case CANOPEN_SDO_BATTERY_VOLTAGE_IDX:
        break;
        case CANOPEN_SDO_BATTERY_CURRENT_IDX:
        break;
        case CANOPEN_SDO_BATTERY_SOC_IDX:
        break;
        case CANOPEN_SDO_THROTTLE_VOLTAGE_IDX:
        break;
        case CANOPEN_SDO_BRAKE_1_VOLTAGE_IDX:
        break;
        case CANOPEN_SDO_BRAKE_2_VOLTAGE_IDX:
        break;
        case CANOPEN_SDO_BATTERY_POWER_PERCENT_IDX:
        break;
    }
}

CanopenSdo_SDO_object_t* CanopenSdo_get(int index)
{
    for (int i=0; i<sizeof(sdo_db) / sizeof(sdo_db[0]); i++) {
        if (sdo_db[i].index == index) {
            return &sdo_db[i];
            break;
        }
    }
    return NULL;
}
