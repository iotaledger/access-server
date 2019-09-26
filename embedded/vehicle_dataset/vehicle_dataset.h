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
 * \file vehicle_dataset.h
 * \brief
 * Vehicle dataset interface header file
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 26.06.2019. Initial version.
 ****************************************************************************/
#ifndef _VEHICLE_DATASET_H_
#define _VEHICLE_DATASET_H_

#include <stddef.h>
#include <stdint.h>

extern char VehicleDatasetCan01_options[];
extern char VehicleDatasetCanopen01_options[];

typedef struct {
	char* name;
	int val;
} vd_token_t;

typedef struct {
    char* options;
    int options_count;
    vd_token_t* tokens;
    void* dataset;
} VehicleDataset_state_t;

typedef struct {
	uint8_t DoorDrvrSts;
	uint8_t DoorDrvrReSts;
	uint8_t VehMtnStSafe;
	uint8_t EngCooltLvl;
	uint8_t TrSts;
	uint8_t DoorPassSts;
	uint8_t DoorPassReSts;
	uint8_t LockgCenStsForUsrFb;
	uint8_t VehSpdLgtSafe;
	uint8_t FuLvlIndcdVal;
	uint8_t EngN;
	uint8_t BkpOfDstTrvld;
	uint8_t AmbTIndcd;
	uint8_t VehSpdIndcdVal;
	uint8_t VinBlk;
	uint8_t VinPosn1;
	uint8_t VinPosn2;
	uint8_t VinPosn3;
	uint8_t VinPosn4;
	uint8_t VinPosn5;
	uint8_t VinPosn6;
	uint8_t VinPosn7;
	uint8_t TrLockSts;
	uint8_t VehBattUSysU;
	uint8_t AmbTValAtDrvrSide;
	uint8_t AmbTURawAtLeSideForObd;
	uint8_t AmbTURawAtRiSideForObd;
	uint8_t AmbTValAtPassSide;
	uint8_t DrvrBrkTqAtWhlsReqd;
	uint8_t BrkPedlPsd;
	uint8_t BrkTqTotReqForPt;
	uint8_t GearIndcn;
	uint8_t DrvrPrpsnTqReq;
	uint8_t CluPedlRat;
} can01_vehicle_dataset_t;

typedef struct {
    uint8_t bms_bat_voltage;
    uint8_t analogue_brake_full_voltage;
    uint8_t analogue_brake_off_voltage;
    uint8_t controller_temperature;
    uint8_t vehicle_speed;
    uint8_t motor_rpm;
    uint8_t motor_speed;
    uint8_t battery_voltage;
    uint8_t battery_current;
    uint8_t battery_soc;
    uint8_t throttle_voltage;
    uint8_t brake_1_voltage;
    uint8_t brake_2_voltage;
    uint8_t battery_power_percent;
    uint8_t raw_battery_voltage;
    uint8_t wheel_rpm_speed_sensor_based;
    uint8_t wheel_rpm_motor_based;
    uint8_t trip_meter;
    uint8_t brake_light_pwm_daylight;
    uint8_t remote_throttle_voltage;
} canopen01_vehicle_dataset_t;

void VehicleDataset_init(VehicleDataset_state_t *state);

void VehicleDataset_deinit(VehicleDataset_state_t *state);
void VehicleDataset_from_json(VehicleDataset_state_t *state, const char* json, size_t json_len);
int VehicleDataset_to_json(VehicleDataset_state_t *state, char* json);
int VehicleDataset_checked_count(VehicleDataset_state_t *state);

#endif
