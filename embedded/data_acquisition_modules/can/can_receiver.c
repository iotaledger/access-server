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
 * \file can_receiver.h
 * \brief
 * CAN receiver server implementation
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include "can_linux.h"
#include "can_msgs.h"

#include "can_receiver.h"
#include "json_interface.h"

static void be2le(unsigned char *in, unsigned char *out)
{
    unsigned char data[8];
    for (int i=0; i<8; i++){
        data[i] = in[7-i];
    }
    for (int i=0; i<8; i++){
        out[i] = data[i];
    }
}

static can01_vehicle_dataset_t *wanted_signals;
static int data_available = 0;

// CAN data stuff
static double latitude = -1.;
static double longitude = -1.;
static double speed = -1.;
static fjson_object *fj_obj_can;
static fjson_object *fj_values_array_ambient_air_temperature;
static fjson_object *fj_values_array_fuel_tank_level;
static fjson_object *fj_values_array_lock_status;
static fjson_object *fj_values_array_trunk_status;
static fjson_object *fj_values_array_driver_door_status;
static fjson_object *fj_values_array_driver_door_rear_status;
static fjson_object *fj_values_array_passenger_door_status;
static fjson_object *fj_values_array_passenger_door_rear_status;
static fjson_object *fj_values_array_requested_brake_torque_at_wheels;
static fjson_object *fj_values_array_requested_propulsion_torque;
static fjson_object *fj_values_array_clutch_pedal_position;
static fjson_object *fj_values_array_brake_pedal_pressed;

#define AmbTIndcd_JSON_NAME "ambient_air_temperature"
#define FuLvlIndcdVal_JSON_NAME "fuel_tank_level"
#define LockgCenStsForUsrFb_JSON_NAME "central_locking_status_for_user_feedback"
#define TrSts_JSON_NAME "trunk_status"
#define DoorDrvrSts_JSON_NAME "driver_door_status"
#define DoorDrvrReSts_JSON_NAME "driver_door_rear_status"
#define DoorPassSts_JSON_NAME "passenger_door_status"
#define DoorPassReSts_JSON_NAME "passenger_door_rear_status"
#define DrvrBrkTqAtWhlsReqd_JSON_NAME "requested_brake_torque_at_wheels"
#define DrvrPrpsnTqReq_JSON_NAME "requested_propulsion_torque"
#define CluPedlRat_JSON_NAME "clutch_pedal_position"
#define BrkPedlPsd_JSON_NAME "brake_pedal_pressed"

static fjson_object* can_json_filler()
{
    fjson_object *fj_data_array = fjson_object_new_array();

    data_available = 0;

    if (wanted_signals->AmbTIndcd == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(AmbTIndcd_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_ambient_air_temperature = fj_obj_tmp;
        data_available = 1;
    }
    else
    {
        fj_values_array_ambient_air_temperature = NULL;
    }

    if (wanted_signals->FuLvlIndcdVal == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(FuLvlIndcdVal_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_fuel_tank_level = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_fuel_tank_level = NULL;
    }

    if (wanted_signals->LockgCenStsForUsrFb == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(LockgCenStsForUsrFb_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_lock_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_lock_status = NULL;
    }

    if (wanted_signals->TrSts == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(TrSts_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_trunk_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_trunk_status = NULL;
    }

	if (wanted_signals->DoorDrvrSts == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorDrvrSts_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_driver_door_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_driver_door_status = NULL;
    }

    if (wanted_signals->DoorDrvrReSts == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorDrvrReSts_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_driver_door_rear_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_driver_door_rear_status = NULL;
    }

    if (wanted_signals->DoorPassSts == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorPassSts_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_passenger_door_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_passenger_door_status = NULL;
    }

    if (wanted_signals->DoorPassReSts == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorPassReSts_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_passenger_door_rear_status = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_passenger_door_rear_status = NULL;
    }

    if (wanted_signals->DrvrBrkTqAtWhlsReqd == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DrvrBrkTqAtWhlsReqd_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_requested_brake_torque_at_wheels = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_requested_brake_torque_at_wheels = NULL;
    }

    if (wanted_signals->DrvrPrpsnTqReq == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DrvrPrpsnTqReq_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_requested_propulsion_torque = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_requested_propulsion_torque = NULL;
    }

    if (wanted_signals->CluPedlRat == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(CluPedlRat_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_clutch_pedal_position = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_clutch_pedal_position = NULL;
    }

    if (wanted_signals->BrkPedlPsd == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(BrkPedlPsd_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_brake_pedal_pressed = fj_obj_tmp;
        data_available = 1;
    } else {
        fj_values_array_brake_pedal_pressed = NULL;
    }

    return fj_data_array;
}
#define CAN_JSON_NAME "can_data"
// !CAN data stuff

static pthread_mutex_t *json_sync_lock;
static CanThread_instance_t can_chas_instance;
static CanThread_instance_t can_body_instance;
static void can_body_frame_read_cb(struct can_frame *frame);
static void can_chas_frame_read_cb(struct can_frame *frame);

void CanReceiver_init(const char* can_body_channel, const char* can_chas_channel, can01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex)
{
    wanted_signals = dataset;
    JSONInterface_add_module_init_cb(can_json_filler, &fj_obj_can, CAN_JSON_NAME);
    json_sync_lock = json_mutex;
    CanThread_init(&can_body_instance, can_body_channel, can_body_frame_read_cb);
    CanThread_init(&can_chas_instance, can_chas_channel, can_chas_frame_read_cb);
}

void CanReceiver_start()
{
    CanThread_start(&can_body_instance);
    CanThread_start(&can_chas_instance);
}

int CanReceiver_deinit()
{
    if (CanThread_stop(&can_chas_instance)) return 1;
    if (CanThread_stop(&can_body_instance)) return 2;
    return 0;
}

static fjson_object* gen_interpreted_door_status(int status) {
    fjson_object* fj_value = fjson_object_new_object();
    fjson_object* fj_value_string;
    switch (status) {
        case 0x0:
            fj_value_string = fjson_object_new_string("Unknown");
            break;
        case 0x1:
            fj_value_string = fjson_object_new_string("Opened");
            break;
        case 0x2:
            fj_value_string = fjson_object_new_string("Closed");
            break;
        default:
            fj_value_string = fjson_object_new_string("ERROR");
    }
    fjson_object_object_add(fj_value, "value", fj_value_string);
    return fj_value;
}

static fjson_object* gen_interpreted_lock_status(int status) {
    fjson_object* fj_value = fjson_object_new_object();
    fjson_object* fj_value_string;
    switch (status) {
        case 0x0:
            fj_value_string = fjson_object_new_string("Undefined");
            break;
        case 0x1:
            fj_value_string = fjson_object_new_string("Opened");
            break;
        case 0x2:
            fj_value_string = fjson_object_new_string("Closed");
            break;
        case 0x3:
            fj_value_string = fjson_object_new_string("Locked");
            break;
        case 0x4:
            fj_value_string = fjson_object_new_string("Safe");
            break;
        default:
            fj_value_string = fjson_object_new_string("ERROR");
    }
    fjson_object_object_add(fj_value, "value", fj_value_string);
    return fj_value;
}

static fjson_object* gen_interpreted_temperature_unit(int status) {
    fjson_object* fj_value = fjson_object_new_object();
    fjson_object* fj_value_string;
    switch (status) {
        case 0x0:
            fj_value_string = fjson_object_new_string("C");
            break;
        case 0x1:
            fj_value_string = fjson_object_new_string("F");
            break;
        case 0x2:
            fj_value_string = fjson_object_new_string("Unknown unit");
            break;
        default:
            fj_value_string = fjson_object_new_string("ERROR");
    }
    fjson_object_object_add(fj_value, "value", fj_value_string);
    return fj_value;
}

static void can_body_frame_read_cb(struct can_frame *frame)
{
    char can_frame_string[1024];
    be2le(frame->data, frame->data);
    sprint_canframe(&can_frame_string[0], frame, 1);
    //fprintf(stdout, "read can frame: %s\n", can_frame_string);
    unsigned canid = frame->can_id&0x7ff;
    if (CanMsgs_body_msg_supported(canid)) {
        CanMsgs_BodyMessage_t bm;
        memcpy(&bm, frame->data, 8);

        pthread_mutex_lock(json_sync_lock);
        switch (canid) {
            case 0x40:
                if (bm.msg_0x40.DoorDrvrReSts_UB == 1 && wanted_signals->DoorDrvrReSts == 1 && fj_values_array_driver_door_rear_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_driver_door_rear_status, "value", gen_interpreted_door_status(bm.msg_0x40.DoorDrvrReSts));
                }
                if (bm.msg_0x40.DoorDrvrSts_UB == 1 && wanted_signals->DoorDrvrSts == 1 && fj_values_array_driver_door_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_driver_door_status, "value", gen_interpreted_door_status(bm.msg_0x40.DoorDrvrSts));
                }
                if (bm.msg_0x40.VehMtnStSafeGroup_UB == 1)
                {
                    //emit_signal("VehMtnStSafe", bm.msg_0x40.VehMtnStSafe, -1);
                }
                break;
            case 0xB0:
                if (bm.msg_0xB0.EngCooltLvl_UB == 1)
                {
                    //emit_signal("EngCooltLvl", bm.msg_0xB0.EngCooltLvl, -1);
                }
                break;
            case 0xE0:
                if (bm.msg_0xE0.TrSts_UB == 1 && wanted_signals->TrSts == 1 && fj_values_array_trunk_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_trunk_status, "value", gen_interpreted_door_status(bm.msg_0xE0.TrSts));
                }

                if (bm.msg_0xE0.DoorPassSts_UB == 1 && wanted_signals->DoorPassSts && fj_values_array_passenger_door_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_passenger_door_status, "value", gen_interpreted_door_status(bm.msg_0xE0.DoorPassSts));
                }

                if (bm.msg_0xE0.DoorPassReSts_UB == 1 && wanted_signals->DoorPassReSts && fj_values_array_passenger_door_rear_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_passenger_door_rear_status, "value", gen_interpreted_door_status(bm.msg_0xE0.DoorPassReSts));
                }
                break;
            case 0x100:
                if (bm.msg_0x100.LockgCenStsForUsrFb_UB == 1 && wanted_signals->LockgCenStsForUsrFb == 1 && fj_values_array_lock_status != NULL)
                {
                    fjson_object* fj_value = fjson_object_new_object();
                    fjson_object_object_add(fj_value, "value", gen_interpreted_lock_status(bm.msg_0x100.LockgCenStsForUsrFb));
                    fjson_object_object_add(fj_values_array_lock_status, "value", fj_value);
                }
                break;
            case 0x1B0:
                if (bm.msg_0x1B0.VehSpdLgtSafeGroup_UB == 1)
                {
                    //emit_signal("VehSpdLgtSafe", bm.msg_0x1B0.VehSpdLgtSafe, -1);
                }
                break;
            case 0x270:
                if (bm.msg_0x270.FuLvlIndcd_UB == 1 && wanted_signals->FuLvlIndcdVal == 1 && fj_values_array_fuel_tank_level != NULL)
                {
                    // minimum: 0, maximum: 204.6, factor: 0.2
                    fjson_object* fj_value = fjson_object_new_object();
                    fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)bm.msg_0x270.FuLvlIndcdVal * 0.2));
                    fjson_object_object_add(fj_value, "unit", fjson_object_new_string("l"));
                    fjson_object_object_add(fj_values_array_fuel_tank_level, "value", fj_value);
                }
            case 0x300:
                if (bm.msg_0x300.EngNSafe_UB == 1)
                {
                    //emit_signal("EngN", bm.msg_0x300.EngN, -1);
                }
                break;
            case 0x310:
                if (bm.msg_0x310.BkpOfDstTrvld_UB == 1)
                {
                    //emit_signal("BkpOfDstTrvld", bm.msg_0x310.BkpOfDstTrvld, -1);
                }
                break;
            case 0x1D0:
                if (bm.msg_0x1D0.AmbTIndcdWithUnit_UB == 1 && wanted_signals->AmbTIndcd == 1 && fj_values_array_ambient_air_temperature != NULL)
                {
                    fjson_object* fj_value = fjson_object_new_object();
                    fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)bm.msg_0x1D0.AmbTIndcd * 0.1 - 100.));
                    fjson_object_object_add(fj_value, "unit", gen_interpreted_temperature_unit(bm.msg_0x1D0.AmbTIndcdUnit));
                    fjson_object_object_add(fj_values_array_ambient_air_temperature, "value", fj_value);
                }
                if (bm.msg_0x1D0.VehSpdIndcd_UB == 1)
                {
                    //emit_signal("VehSpdIndcdVal", bm.msg_0x1D0.VehSpdIndcdVal, bm.msg_0x1D0.VehSpdIndcdUnit);
                }
                break;
            case 0x360:
                //emit_signal("VinPosn7", bm.msg_0x360.VinPosn7, -1);
                //emit_signal("VinPosn6", bm.msg_0x360.VinPosn6, -1);
                //emit_signal("VinPosn5", bm.msg_0x360.VinPosn5, -1);
                //emit_signal("VinPosn4", bm.msg_0x360.VinPosn4, -1);
                //emit_signal("VinPosn3", bm.msg_0x360.VinPosn3, -1);
                //emit_signal("VinPosn2", bm.msg_0x360.VinPosn2, -1);
                //emit_signal("VinPosn1", bm.msg_0x360.VinPosn1, -1);
                //emit_signal("VinBlk", bm.msg_0x360.VinBlk, -1);
                break;
            case 0x380:
                if (bm.msg_0x380.TrLockSts_UB == 1)
                {
                    //emit_signal("TrLockSts", bm.msg_0x380.TrLockSts, -1);
                }
                break;
            case 0x330:
                if (bm.msg_0x330.VehBattUGroup_UB == 1)
                {
                    //emit_signal("VehBattUSysU", bm.msg_0x330.VehBattUSysU, -1);
                }
                break;
            case 0x1E0:
                if (bm.msg_0x1E0.AmbTRawAtDrvrSide_UB == 1)
                {
                    //emit_signal("AmbTValAtDrvrSide", bm.msg_0x1E0.AmbTValAtDrvrSide, -1);
                }
                break;
            case 0x130:
                if (bm.msg_0x130.AmbTURawAtLeSideForObd_UB == 1)
                {
                    //emit_signal("AmbTURawAtLeSideForObd", bm.msg_0x130.AmbTURawAtLeSideForObd, -1);
                }
                break;
            case 0x160:
                if (bm.msg_0x160.AmbTURawAtRiSideForObd_UB == 1)
                {
                    //emit_signal("AmbTURawAtRiSideForObd", bm.msg_0x160.AmbTURawAtRiSideForObd, -1);
                }
                if (bm.msg_0x160.AmbTRawAtPassSide_UB == 1)
                {
                    //emit_signal("AmbTValAtPassSide", bm.msg_0x160.AmbTValAtPassSide, -1);
                }
                break;
        }
        pthread_mutex_unlock(json_sync_lock);
    }
    JSONInterface_dump_if_needed(6, 1);
}

static void can_chas_frame_read_cb(struct can_frame *frame)
{
    char can_frame_string[1024];
    be2le(frame->data, frame->data);
    sprint_canframe(&can_frame_string[0], frame, 1);
    //fprintf(stdout, "read can frame: %s\n", can_frame_string);
    unsigned canid = frame->can_id&0x7ff;
    if (CanMsgs_chas_msg_supported(canid)) {
        CanMsgs_ChasMessage_t cm;
        memcpy(&cm, frame->data, 8);

        pthread_mutex_lock(json_sync_lock);
        switch (canid) {
            case 0xE0:
                if (cm.msg_0xE0.DrvrBrkTqAtWhlsReqdGroup_UB == 1 && wanted_signals->DrvrBrkTqAtWhlsReqd == 1 && fj_values_array_requested_brake_torque_at_wheels != NULL)
                {
                    fjson_object_object_add(fj_values_array_requested_brake_torque_at_wheels, "value", fjson_object_new_int(cm.msg_0xE0.DrvrBrkTqAtWhlsReqd));
                }
                if (cm.msg_0xE0.VehSpdLgtSafeGroup_UB == 1)
                {
                    //emit_signal("VehSpdLgtSafe", cm.msg_0xE0.VehSpdLgtSafe, -1);
                }
                break;
            case 0xF0:
                if (cm.msg_0xF0.TrSts_UB == 1 && wanted_signals->TrSts == 1 && fj_values_array_trunk_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_trunk_status, "value", gen_interpreted_door_status(cm.msg_0xF0.TrSts));
                }
                if (cm.msg_0xF0.DoorPassReSts_UB == 1 && wanted_signals->DoorPassReSts == 1 && fj_values_array_passenger_door_rear_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_passenger_door_rear_status, "value", gen_interpreted_door_status(cm.msg_0xF0.DoorPassReSts));
                }
                if (cm.msg_0xF0.DoorDrvrSts_UB == 1 && wanted_signals->DoorDrvrSts == 1 && fj_values_array_driver_door_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_driver_door_status, "value", gen_interpreted_door_status(cm.msg_0xF0.DoorDrvrSts));
                }
                if (cm.msg_0xF0.DoorPassSts_UB == 1 && wanted_signals->DoorPassSts == 1 && fj_values_array_passenger_door_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_passenger_door_status, "value", gen_interpreted_door_status(cm.msg_0xF0.DoorPassSts));
                }
                break;
            case 0x3A0:
                if (cm.msg_0x3A0.DoorDrvrReSts_UB == 1 && wanted_signals->DoorDrvrReSts == 1 && fj_values_array_driver_door_rear_status != NULL)
                {
                    fjson_object_object_add(fj_values_array_driver_door_rear_status, "value", gen_interpreted_door_status(cm.msg_0x3A0.DoorDrvrReSts));
                }

                if (cm.msg_0x3A0.AmbTIndcdWithUnit_UB == 1 && wanted_signals->AmbTIndcd == 1 && fj_values_array_ambient_air_temperature != NULL)
                {
                    // minimum: -100, maximum: 309.5, factor: 0.1, offset: 100
                    fjson_object* fj_value = fjson_object_new_object();
                    fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)cm.msg_0x3A0.AmbTIndcd * 0.1 - 100.));
                    fjson_object_object_add(fj_value, "unit", gen_interpreted_temperature_unit(cm.msg_0x3A0.AmbTIndcdUnit));
                    fjson_object_object_add(fj_values_array_ambient_air_temperature, "value", fj_value);
                }
                break;
            case 0x1C0:
                if (cm.msg_0x1C0.BrkPedlPsdSafeGroup_UB == 1 && wanted_signals->BrkPedlPsd == 1 && fj_values_array_brake_pedal_pressed != NULL)
                {
                    //emit_signal("BrkPedlPsd", cm.msg_0x1C0.BrkPedlPsd, -1);
                    fjson_object_object_add(fj_values_array_brake_pedal_pressed, "value", fjson_object_new_int(cm.msg_0x1C0.BrkPedlPsd));
                }
                break;
            case 0x1F0:
                if (cm.msg_0x1F0.BkpOfDstTrvld_UB == 1)
                {
                    //emit_signal("BkpOfDstTrvld", cm.msg_0x1F0.BkpOfDstTrvld, -1);
                }
                break;
            case 0x240:
                if (cm.msg_0x240.FuLvlIndcd_UB == 1 && wanted_signals->FuLvlIndcdVal == 1 && fj_values_array_fuel_tank_level != NULL)
                {
                    // minimum: 0, maximum: 204.6, factor: 0.2
                    fjson_object* fj_value = fjson_object_new_object();
                    fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)cm.msg_0x240.FuLvlIndcdVal * 0.2));
                    fjson_object_object_add(fj_value, "unit", fjson_object_new_string("l"));
                    fjson_object_object_add(fj_values_array_fuel_tank_level, "value", fj_value);
                }
                break;
            case 0x380:
                if (cm.msg_0x380.VehBattUGroup_UB == 1)
                {
                    //emit_signal("VehBattUSysU", cm.msg_0x380.VehBattUSysU, -1);
                }
                if (cm.msg_0x380.EngCooltLvl_UB == 1)
                {
                    //emit_signal("EngCooltLvl", cm.msg_0x380.EngCooltLvl, -1);
                }
                break;
            case 0x1A:
                if (cm.msg_0x1A.BrkTq_UB == 1)
                {
                    //emit_signal("BrkTqTotReqForPt", cm.msg_0x1A.BrkTqTotReqForPt, -1);
                }
                break;
            case 0x100:
                if (cm.msg_0x100.GearIndcnRec_UB == 1)
                {
                    //emit_signal("GearIndcn", cm.msg_0x100.GearIndcn, -1);
                }
                break;
            case 0x130:
                if (cm.msg_0x130.DrvrPrpsnTqReq_UB == 1 && wanted_signals->DrvrPrpsnTqReq == 1 && fj_values_array_requested_propulsion_torque != NULL)
                {
                    fjson_object_object_add(fj_values_array_requested_propulsion_torque, "value", fjson_object_new_int(cm.msg_0x130.DrvrPrpsnTqReq));
                }
                if (cm.msg_0x130.EngNSafe_UB == 1)
                {
                    //emit_signal("EngN", cm.msg_0x130.EngN, -1);
                }
                break;
            case 0x210:
                if (cm.msg_0x210.CluPedlRat_UB == 1 && wanted_signals->CluPedlRat == 1 && fj_values_array_clutch_pedal_position != NULL)
                {
                    fjson_object_object_add(fj_values_array_clutch_pedal_position, "value", fjson_object_new_double((double)cm.msg_0x210.CluPedlRat * 0.392156862745));
                }
                break;
        }
        pthread_mutex_unlock(json_sync_lock);
    }
    JSONInterface_dump_if_needed(6, 1);
}
