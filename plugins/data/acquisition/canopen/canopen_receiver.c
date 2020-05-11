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
 * \file canopen_receiver.c
 * \brief
 * Implementation of interface for CANOPEN receiver
 *
 * @Author
 *
 * \notes
 *
 * \history
 * XX.YY.ZZZZ. Initial version.
 ****************************************************************************/

#include "canopen_receiver.h"
#include "can_thread.h"
#include "canopen_sdo.h"
#include "json_interface.h"

#include "globals_declarations.h"

#include <unistd.h>
#include <string.h>


typedef enum {
	CANOPEN_SHD_INITIALIZING = 0x00,
	CANOPEN_SHD_STOPPED = 0x04,
	CANOPEN_SHD_OPERATIONAL = 0x05,
	CANOPEN_SHD_PREOPERATIONAL = 0x7f
} Canopen_service_heartbeat_data_e;

typedef enum {
	CANOPEN_STC_4BYTES = 0x43,
	CANOPEN_STC_3BYTES = 0x47,
	CANOPEN_STC_2BYTES = 0x4b,
	CANOPEN_STC_1BYTE = 0x4f
} Canopen_service_tsdo_command_e;

#define CANOPEN_JSON_NAME "canopen_data"
#define CANOPEN_SERVICE_TPDO1 0x180
#define CANOPEN_SERVICE_TSDO 0x580
#define CANOPEN_SERVICE_HEARTBEAT 0x700
#define CANOPEN_NONODE_ARRAY_SIZE 8
#define CANOPEN_NONODE_STRING_SIZE 8
#define CANOPEN_SERVICE_NUM 12
#define CANOPEN_SHD_PREOP_CANID 0
#define CANOPEN_SHD_PREOP_CANDLC 2
#define CANOPEN_SHD_PREOP_CANDATA0 0x1
#define CANOPEN_MAX_STR_SIZE 256
#define CANOPEN_JSON_DUMP_PERIOD_6S 6
#define CANOPEN_SYNC_COUNTER_PRESCALER 100000
#define CANOPEN_SYNC_CANID 0x80
#define CANOPEN_SYNC_CANDLC 0
#define CANOPEN_READ_COUNTER_PRESCALER 400000
#define CANOPEN_READ_CANIDBASE 0x600
#define CANOPEN_READ_CANDLC 8
#define CANOPEN_READ_DATA_READ_COMMAND 0x40

#define CANOPEN_FILL_FRAME_DATA(X) {frame.data[1] = ((X) & 0x00ff00) >> 8; frame.data[2] = ((X) & 0xff0000) >> 16; frame.data[3] = (X) & 0xff; CAN_send_frame(&can_instance.can_connection, &frame); usleep(g_task_sleep_time);}

static canopen01_vehicle_dataset_t *wanted_signals;
static int end_loop = 0;
static bool is_in_use = FALSE;
static char port_name[CANOPEN_MAX_STR_SIZE];

// CANopen data stuff

static fjson_object *fj_obj_canopen;
static fjson_object *fj_values_array_bms_bat_voltage;
static fjson_object *fj_values_array_analogue_brake_full_voltage;
static fjson_object *fj_values_array_analogue_brake_off_voltage;
static fjson_object *fj_values_array_controller_temperature;
static fjson_object *fj_values_array_vehicle_speed;
static fjson_object *fj_values_array_motor_rpm;
static fjson_object *fj_values_array_motor_speed;
static fjson_object *fj_values_array_battery_voltage;
static fjson_object *fj_values_array_battery_current;
static fjson_object *fj_values_array_battery_soc;
static fjson_object *fj_values_array_throttle_voltage;
static fjson_object *fj_values_array_brake_1_voltage;
static fjson_object *fj_values_array_brake_2_voltage;
static fjson_object *fj_values_array_battery_power_percent;
static fjson_object *fj_values_array_raw_battery_voltage;
static fjson_object *fj_values_array_wheel_rpm_speed_sensor_based;
static fjson_object *fj_values_array_wheel_rpm_motor_based;
static fjson_object *fj_values_array_trip_meter;
static fjson_object *fj_values_array_remote_throttle_voltage;

#define bms_bat_voltage_JSON_NAME "bms_bat_voltage"
#define analogue_brake_full_voltage_JSON_NAME "analogue_brake_full_voltage"
#define analogue_brake_off_voltage_JSON_NAME "analogue_brake_off_voltage"
#define controller_temperature_JSON_NAME "controller_temperature"
#define vehicle_speed_JSON_NAME "vehicle_speed"
#define motor_rpm_JSON_NAME "motor_rpm"
#define motor_speed_JSON_NAME "motor_speed"
#define battery_voltage_JSON_NAME "battery_voltage"
#define battery_current_JSON_NAME "battery_current"
#define battery_soc_JSON_NAME "battery_soc"
#define throttle_voltage_JSON_NAME "throttle_voltage"
#define brake_1_voltage_JSON_NAME "brake_1_voltage"
#define brake_2_voltage_JSON_NAME "brake_2_voltage"
#define battery_power_percent_JSON_NAME "battery_power_percent"
#define raw_battery_voltage_JSON_NAME "raw_battery_voltage"
#define wheel_rpm_speed_sensor_based_JSON_NAME "wheel_rpm_speed_sensor_based"
#define wheel_rpm_motor_based_JSON_NAME "wheel_rpm_motor_based"
#define trip_meter_JSON_NAME "trip_meter"
#define remote_throttle_voltage_JSON_NAME "remote_throttle_voltage"

static fjson_object* can_json_filler()
{
    fjson_object *fj_data_array = fjson_object_new_array();

    if (wanted_signals->bms_bat_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(bms_bat_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_bms_bat_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_bms_bat_voltage = NULL;
    }

    if (wanted_signals->analogue_brake_full_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(analogue_brake_full_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_analogue_brake_full_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_analogue_brake_full_voltage = NULL;
    }

    if (wanted_signals->analogue_brake_off_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(analogue_brake_off_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_analogue_brake_off_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_analogue_brake_off_voltage = NULL;
    }

    if (wanted_signals->controller_temperature == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(controller_temperature_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_controller_temperature = fj_obj_tmp;
    }
    else
    {
        fj_values_array_controller_temperature = NULL;
    }

    if (wanted_signals->vehicle_speed == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(vehicle_speed_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_vehicle_speed = fj_obj_tmp;
    }
    else
    {
        fj_values_array_vehicle_speed = NULL;
    }

    if (wanted_signals->motor_rpm == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(motor_rpm_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_motor_rpm = fj_obj_tmp;
    }
    else
    {
        fj_values_array_motor_rpm = NULL;
    }

    if (wanted_signals->motor_speed == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(motor_speed_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_motor_speed = fj_obj_tmp;
    }
    else
    {
        fj_values_array_motor_speed = NULL;
    }

    if (wanted_signals->battery_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(battery_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_battery_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_battery_voltage = NULL;
    }

    if (wanted_signals->battery_current == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(battery_current_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_battery_current = fj_obj_tmp;
    }
    else
    {
        fj_values_array_battery_current = NULL;
    }

    if (wanted_signals->battery_soc == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(battery_soc_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_battery_soc = fj_obj_tmp;
    }
    else
    {
        fj_values_array_battery_soc = NULL;
    }

    if (wanted_signals->throttle_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(throttle_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_throttle_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_throttle_voltage = NULL;
    }

    if (wanted_signals->brake_1_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(brake_1_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_brake_1_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_brake_1_voltage = NULL;
    }

    if (wanted_signals->brake_2_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(brake_2_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_brake_2_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_brake_2_voltage = NULL;
    }

    if (wanted_signals->battery_power_percent == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(battery_power_percent_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_battery_power_percent = fj_obj_tmp;
    }
    else
    {
        fj_values_array_battery_power_percent = NULL;
    }

    if (wanted_signals->raw_battery_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(raw_battery_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_raw_battery_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_raw_battery_voltage = NULL;
    }

    if (wanted_signals->wheel_rpm_speed_sensor_based == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(wheel_rpm_speed_sensor_based_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_wheel_rpm_speed_sensor_based = fj_obj_tmp;
    }
    else
    {
        fj_values_array_wheel_rpm_speed_sensor_based = NULL;
    }

    if (wanted_signals->wheel_rpm_motor_based == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(wheel_rpm_motor_based_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_wheel_rpm_motor_based = fj_obj_tmp;
    }
    else
    {
        fj_values_array_wheel_rpm_motor_based = NULL;
    }

    if (wanted_signals->trip_meter == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(trip_meter_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_trip_meter = fj_obj_tmp;
    }
    else
    {
        fj_values_array_trip_meter = NULL;
    }

    if (wanted_signals->remote_throttle_voltage == 1) {
        fjson_object* fj_obj_tmp = fjson_object_new_object();
        fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(remote_throttle_voltage_JSON_NAME));
        fjson_object_array_add(fj_data_array, fj_obj_tmp);
        fj_values_array_remote_throttle_voltage = fj_obj_tmp;
    }
    else
    {
        fj_values_array_remote_throttle_voltage = NULL;
    }

    return fj_data_array;
}

// !CANopen data stuff

static pthread_mutex_t *json_sync_lock;
static pthread_t canopen_bg_thread;
static int node_id;
static CanThread_instance_t can_instance;

static const int services_nonodeid [CANOPEN_NONODE_ARRAY_SIZE] = {
    0,
    0x80,
    0x100
};

static const char services_nonodeid_names[CANOPEN_NONODE_ARRAY_SIZE][CANOPEN_NONODE_STRING_SIZE] = {
    "NMT",
    "SYNC",
    "TIME"
};

static const int services[CANOPEN_SERVICE_NUM] = {
    0x080,
    0x180,
    0x200,
    0x280,
    0x300,
    0x380,
    0x400,
    0x480,
    0x500,
    0x580,
    0x600,
    0x700
};

static const char services_names[CANOPEN_SERVICE_NUM][CANOPEN_NONODE_STRING_SIZE] = {
    "EMCY",
    "TPDO1",
    "RPDO1",
    "TPDO2",
    "RPDO2",
    "TPDO3",
    "RPDO3",
    "TPDO4",
    "RPDO4",
    "TSDO",
    "RSDO",
    "NMT-EC"
};

static void can_read_callback(struct can_frame *frame)
{
    int found = -1;
    for (int i=0; i<CANOPEN_NONODE_ARRAY_SIZE; i++){
        if (frame->can_id == services_nonodeid[i]) {
            found = i;
        }
    }

    if (found >= 0) {
        printf("CANopen: %s [%d] ", services_nonodeid_names[found], frame->can_dlc);
        for (int i=0; i<frame->can_dlc; i++) {
            printf("%02X", frame->data[i]);
        }
        printf("\n");
        return;
    }

    if (found == -1){
        int service = frame->can_id - node_id;
        for (int i=0; i<CANOPEN_SERVICE_NUM; i++){
            if (service == services[i]) {
                found = i;
            }
        }

        if (found >= 0){
            switch (service) {
                case CANOPEN_SERVICE_HEARTBEAT:
                    //printf("CANopen - heartbeat: node %02X -", node_id);
                    switch (frame->data[0]){
                        case CANOPEN_SHD_INITIALIZING:
                            printf(" boot up (initializing)\n");
                        break;
                        case CANOPEN_SHD_STOPPED:
                            printf(" stopped\n");
                        break;
                        case CANOPEN_SHD_OPERATIONAL:
                            //printf(" operational\n");
                        break;
                        case CANOPEN_SHD_PREOPERATIONAL:
                            printf(" pre-operational\n");
                            // put into operational
                            struct can_frame _frame;
                            _frame.can_id = CANOPEN_SHD_PREOP_CANID;
                            _frame.can_dlc = CANOPEN_SHD_PREOP_CANDLC;
                            _frame.data[0] = CANOPEN_SHD_PREOP_CANDATA0;
                            _frame.data[1] = node_id;
                            CAN_send_frame(&can_instance.can_connection, &_frame);
                        break;
                        default:
                            printf(" invalid state, should not happen!\n");
                        break;
                    }
                break;
                case CANOPEN_SERVICE_TSDO:
                    {
                        int command = frame->data[0];
                        int index = frame->data[3] + ((int)frame->data[1] << 8) + ((int)frame->data[2] << 16);
                        int data;
                        switch (command) {
                            case CANOPEN_STC_4BYTES:
                                data = frame->data[4] + ((int)frame->data[5] << 8) + ((int)frame->data[6] << 16) + ((int)frame->data[7] << 24);
                            break;
                            case CANOPEN_STC_3BYTES:
                                data = frame->data[4] + ((int)frame->data[5] << 8) + ((int)frame->data[6] << 16);
                            break;
                            case CANOPEN_STC_2BYTES:
                                data = frame->data[4] + ((int)frame->data[5] << 8);
                            break;
                            case CANOPEN_STC_1BYTE:
                                data = frame->data[4];
                            break;
                        }
                        CanopenSdo_parsed_data_t parsed_sdo_data;
                        CanopenSdo_parse(index, data, &parsed_sdo_data);

                        pthread_mutex_lock(json_sync_lock);
                        fjson_object* fj_value = fjson_object_new_object();
                        fjson_object_object_add(fj_value, "value", fjson_object_new_double(parsed_sdo_data.value));
                        fjson_object_object_add(fj_value, "unit", fjson_object_new_string(parsed_sdo_data.unit));
                        int added = 1;
                        if (index == CANOPEN_SDO_BMS_BAT_VOLTAGE_IDX && wanted_signals->bms_bat_voltage == 1 && fj_values_array_bms_bat_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_bms_bat_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_ANALOGUE_BRAKE_FULL_VOLTAGE_IDX && wanted_signals->analogue_brake_full_voltage == 1 && fj_values_array_analogue_brake_full_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_analogue_brake_full_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_ANALOGUE_BRAKE_OFF_VOLTAGE_IDX && wanted_signals->analogue_brake_off_voltage == 1 && fj_values_array_analogue_brake_off_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_analogue_brake_off_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_CONTROLLER_TEMPERATURE_IDX && wanted_signals->controller_temperature == 1 && fj_values_array_controller_temperature != NULL) {
                            fjson_object_object_add(fj_values_array_controller_temperature, "value", fj_value);
                        } else if (index == CANOPEN_SDO_VEHICLE_SPEED_IDX && wanted_signals->vehicle_speed == 1 && fj_values_array_vehicle_speed != NULL) {
                            fjson_object_object_add(fj_values_array_vehicle_speed, "value", fj_value);
                        } else if (index == CANOPEN_SDO_MOTOR_RPM_IDX && wanted_signals->motor_rpm == 1 && fj_values_array_motor_rpm != NULL) {
                            fjson_object_object_add(fj_values_array_motor_rpm, "value", fj_value);
                        } else if (index == CANOPEN_SDO_MOTOR_SPEED_IDX && wanted_signals->motor_speed == 1 && fj_values_array_motor_speed != NULL) {
                            fjson_object_object_add(fj_values_array_motor_speed, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BATTERY_VOLTAGE_IDX && wanted_signals->battery_voltage == 1 && fj_values_array_battery_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_battery_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BATTERY_CURRENT_IDX && wanted_signals->battery_current == 1 && fj_values_array_battery_current != NULL) {
                            fjson_object_object_add(fj_values_array_battery_current, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BATTERY_SOC_IDX && wanted_signals->battery_soc == 1 && fj_values_array_battery_soc != NULL) {
                            fjson_object_object_add(fj_values_array_battery_soc, "value", fj_value);
                        } else if (index == CANOPEN_SDO_THROTTLE_VOLTAGE_IDX && wanted_signals->throttle_voltage == 1 && fj_values_array_throttle_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_throttle_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BRAKE_1_VOLTAGE_IDX && wanted_signals->brake_1_voltage == 1 && fj_values_array_brake_1_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_brake_1_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BRAKE_2_VOLTAGE_IDX && wanted_signals->brake_2_voltage == 1 && fj_values_array_brake_2_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_brake_2_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_BATTERY_POWER_PERCENT_IDX && wanted_signals->battery_power_percent == 1 && fj_values_array_battery_power_percent != NULL) {
                            fjson_object_object_add(fj_values_array_battery_power_percent, "value", fj_value);
                        } else if (index == CANOPEN_SDO_RAW_BATTERY_VOLTAGE_IDX && wanted_signals->raw_battery_voltage == 1 && fj_values_array_raw_battery_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_raw_battery_voltage, "value", fj_value);
                        } else if (index == CANOPEN_SDO_WHEEL_RPM_SPEED_SENSOR_BASED_IDX && wanted_signals->wheel_rpm_speed_sensor_based == 1 && fj_values_array_wheel_rpm_speed_sensor_based != NULL) {
                            fjson_object_object_add(fj_values_array_wheel_rpm_speed_sensor_based, "value", fj_value);
                        } else if (index == CANOPEN_SDO_WHEEL_RPM_MOTOR_BASED_IDX && wanted_signals->wheel_rpm_motor_based == 1 && fj_values_array_wheel_rpm_motor_based != NULL) {
                            fjson_object_object_add(fj_values_array_wheel_rpm_motor_based, "value", fj_value);
                        } else if (index == CANOPEN_SDO_TRIP_METER_IDX && wanted_signals->trip_meter == 1 && fj_values_array_trip_meter != NULL) {
                            fjson_object_object_add(fj_values_array_trip_meter, "value", fj_value);
                        } else if (index == CANOPEN_SDO_REMOTE_THROTTLE_VOLTAGE_IDX && wanted_signals->remote_throttle_voltage == 1 && fj_values_array_remote_throttle_voltage != NULL) {
                            fjson_object_object_add(fj_values_array_remote_throttle_voltage, "value", fj_value);
                        } else {
                            added = 0;
                        }
                        pthread_mutex_unlock(json_sync_lock);

                    }
                break;
                case CANOPEN_SERVICE_TPDO1:
                    {
                        int voltage;
                        int current;
                        int temperature;
                        int rpm;
                        temperature = frame->data[0] + ((int)frame->data[1] << 8);
                        voltage = frame->data[2] + ((int)frame->data[3] << 8);
                        rpm = frame->data[4] + ((int)frame->data[5] << 8);
                        current = frame->data[6] + ((int)frame->data[7] << 8);
                    }
                break;
                default:
                    printf("CANopen: %s + %02X [%d] ", services_names[found], node_id, frame->can_dlc);
                    for (int i=0; i<frame->can_dlc; i++) {
                        printf("%02X", frame->data[i]);
                    }
                    printf("\n");
                break;
            }
            JSONInterface_dump_if_needed(CANOPEN_JSON_DUMP_PERIOD_6S);
        }
    }
}

#ifndef TINY_EMBEDDED
void CanopenReceiver_preInitSetup(const char *can_interface_name, int _node_id)
{
    is_in_use = TRUE;
    memset(port_name, 0, CANOPEN_MAX_STR_SIZE * sizeof(char));
    memcpy(port_name, can_interface_name, strlen(can_interface_name));
    node_id = _node_id;
}
#endif

void CanopenReceiver_init(canopen01_vehicle_dataset_t *dataset, pthread_mutex_t *json_mutex, const char *can_interface_name, int _node_id)
{
    wanted_signals = dataset;
    JSONInterface_add_module_init_cb(can_json_filler, &fj_obj_canopen, CANOPEN_JSON_NAME);
    json_sync_lock = json_mutex;
    node_id = _node_id;
    CanThread_init(&can_instance, can_interface_name, can_read_callback);
#ifdef TINY_EMBEDDED
    is_in_use = TRUE;
    memset(port_name, 0, CANOPEN_MAX_STR_SIZE * sizeof(char));
    memcpy(port_name, can_interface_name, strlen(can_interface_name));
#endif
}

static void* canopen_bg_thread_func(void* _)
{
    int sync_counter = 0;

    int read_counter = 0;
    while (end_loop != 1) {
        if (sync_counter++ > (CANOPEN_SYNC_COUNTER_PRESCALER / g_task_sleep_time) || (CANOPEN_SYNC_COUNTER_PRESCALER / g_task_sleep_time) == 0) {
            sync_counter = 0;
            struct can_frame frame;
            frame.can_id = CANOPEN_SYNC_CANID;
            frame.can_dlc = CANOPEN_SYNC_CANDLC;
            CAN_send_frame(&can_instance.can_connection, &frame);
        }

        if (read_counter++ > (CANOPEN_READ_COUNTER_PRESCALER / g_task_sleep_time) || (CANOPEN_READ_COUNTER_PRESCALER / g_task_sleep_time) == 0) {
            read_counter = 0;
            struct can_frame frame;
            frame.can_id = CANOPEN_READ_CANIDBASE + node_id;
            frame.can_dlc = CANOPEN_READ_CANDLC;
            frame.data[0] = CANOPEN_READ_DATA_READ_COMMAND;
            frame.data[4] = 0;
            frame.data[5] = 0;
            frame.data[6] = 0;
            frame.data[7] = 0;
            uint8_t *wanted_signals_array = (uint8_t*)&wanted_signals;

            if (wanted_signals->bms_bat_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BMS_BAT_VOLTAGE_IDX);
            }

            if (wanted_signals->analogue_brake_off_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_ANALOGUE_BRAKE_OFF_VOLTAGE_IDX);
            }

            if (wanted_signals->controller_temperature == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_CONTROLLER_TEMPERATURE_IDX);
            }

            if (wanted_signals->vehicle_speed == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_VEHICLE_SPEED_IDX);
            }

            if (wanted_signals->motor_rpm == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_MOTOR_RPM_IDX);
            }

            if (wanted_signals->motor_speed == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_MOTOR_SPEED_IDX);
            }

            if (wanted_signals->battery_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BATTERY_VOLTAGE_IDX);
            }

            if (wanted_signals->battery_current == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BATTERY_CURRENT_IDX);
            }

            if (wanted_signals->battery_soc == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BATTERY_SOC_IDX);
            }

            if (wanted_signals->throttle_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_THROTTLE_VOLTAGE_IDX);
            }

            if (wanted_signals->brake_1_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BRAKE_1_VOLTAGE_IDX);
            }

            if (wanted_signals->brake_2_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BRAKE_2_VOLTAGE_IDX);
            }

            if (wanted_signals->battery_power_percent == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_BATTERY_POWER_PERCENT_IDX);
            }

            if (wanted_signals->raw_battery_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_RAW_BATTERY_VOLTAGE_IDX);
            }

            if (wanted_signals->wheel_rpm_speed_sensor_based == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_WHEEL_RPM_SPEED_SENSOR_BASED_IDX);
            }

            if (wanted_signals->wheel_rpm_motor_based == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_WHEEL_RPM_MOTOR_BASED_IDX);
            }

            if (wanted_signals->trip_meter == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_TRIP_METER_IDX);
            }

            if (wanted_signals->remote_throttle_voltage == 1) {
                CANOPEN_FILL_FRAME_DATA(CANOPEN_SDO_REMOTE_THROTTLE_VOLTAGE_IDX);
            }
#undef CANOPEN_FILL_FRAME_DATA
        }

        usleep(g_task_sleep_time);
    }
}

int CanopenReceiver_start()
{
    if (CanThread_start(&can_instance)){
        fprintf(stderr, "Error creating CANopen read thread\n");
        return 1;
    }

    if (pthread_create(&canopen_bg_thread, NULL, canopen_bg_thread_func, NULL)){
        fprintf(stderr, "Error creating CANopen bg thread\n");
        return 1;
    }
}

void CanopenReceiver_deinit()
{
    end_loop = 1;
    CanThread_stop(&can_instance);
#ifdef TINY_EMBEDDED
    is_in_use = FALSE;
#endif
}

bool CanopenReceiver_isInUse()
{
    return is_in_use;
}

void CanopenReceiver_getPortName(char* p_name_buff, int p_name_buff_len)
{
    if (p_name_buff != NULL && p_name_buff_len > 0)
    {
        memcpy(p_name_buff, port_name, p_name_buff_len > CANOPEN_MAX_STR_SIZE ? CANOPEN_MAX_STR_SIZE : p_name_buff_len);
    }
}

int CanopenReceiver_getNodeId(void)
{
    return node_id;
}
