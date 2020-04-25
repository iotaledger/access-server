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
 * \file modbus_receiver.c
 * \brief
 * Modbus receiver module implementation
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 07.29.2019. Initial version.
 ****************************************************************************/

#include "modbus_receiver.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "json_interface.h"
#include "globals_declarations.h"

typedef struct {
    char serial_device[64];
    fjson_object* fj_root;
    pthread_mutex_t *json_mutex;
    Modbus_t modbus;
} ModbusReceiver_thread_args_t;

static ModbusReceiver_thread_args_t targs;
static canopen01_vehicle_dataset_t *wanted_signals;
static int data_available = 0;

// modbus data stuff
static double rpm = -1.;
static double brake = -1.;
static double throttle_voltage = -1.;
static fjson_object* fj_obj_modbus;
static fjson_object* fj_obj_motor_rpm;
static fjson_object* fj_obj_brake_1_voltage;
static fjson_object* fj_obj_brake_2_voltage;
static fjson_object* fj_obj_throttle_voltage;

static fjson_object* modbus_json_filler()
{
    fj_obj_modbus = fjson_object_new_array();

    if (wanted_signals->motor_rpm == 1) {
        fj_obj_motor_rpm = fjson_object_new_object();
        fjson_object_object_add(fj_obj_motor_rpm, "name", fjson_object_new_string("motor_rpm"));
        fjson_object_array_add(fj_obj_modbus, fj_obj_motor_rpm);
        data_available = 1;
    }
    else
    {
        fj_obj_motor_rpm = NULL;
    }

    if (wanted_signals->brake_1_voltage == 1) {
        fj_obj_brake_1_voltage = fjson_object_new_object();
        fjson_object_object_add(fj_obj_brake_1_voltage, "name", fjson_object_new_string("brake_1_voltage"));
        fjson_object_array_add(fj_obj_modbus, fj_obj_brake_1_voltage);
        data_available = 1;
    }
    else
    {
        fj_obj_brake_1_voltage = NULL;
    }

    if (wanted_signals->brake_2_voltage == 1) {
        fj_obj_brake_2_voltage = fjson_object_new_object();
        fjson_object_object_add(fj_obj_brake_2_voltage, "name", fjson_object_new_string("brake_2_voltage"));
        fjson_object_array_add(fj_obj_modbus, fj_obj_brake_2_voltage);
        data_available = 1;
    }
    else
    {
        fj_obj_brake_1_voltage = NULL;
    }

    if (wanted_signals->throttle_voltage == 1) {
        fj_obj_throttle_voltage = fjson_object_new_object();
        fjson_object_object_add(fj_obj_throttle_voltage, "name", fjson_object_new_string("throttle_voltage"));
        fjson_object_array_add(fj_obj_modbus, fj_obj_throttle_voltage);
        data_available = 1;
    }
    else
    {
        fj_obj_throttle_voltage = NULL;
    }

    return fj_obj_modbus;
}
#define MODBUS_JSON_NAME "modbus_data"
// !modbus data stuff


void ModbusReceiver_init(const char* serial_device,
                         canopen01_vehicle_dataset_t *dataset,
                         pthread_mutex_t *json_mutex)
{
    char buff[128] = {0};
    strncpy(targs.serial_device, serial_device, 64);

    JSONInterface_add_module_init_cb(modbus_json_filler,
                                     &fj_obj_modbus,
                                     MODBUS_JSON_NAME);

    targs.json_mutex = json_mutex;
    wanted_signals = dataset;

    Modbus_init(&targs.modbus, serial_device);
}

static int end_thread = 0;
static pthread_t thread = 0;

#define MODBUS_BRAKE_1_VOLTAGE_REGADDR 0x010f
#define MODBUS_BRAKE_2_VOLTAGE_REGADDR 0x0110
#define MODBUS_RPM_REGADDR 0x107
#define MODBUS_THROTTLE_VOLTAGE_REGADDR 0x010e

static void *thread_loop(void *ptr)
{
    ModbusReceiver_thread_args_t* targs = (ModbusReceiver_thread_args_t*)ptr;

    while (end_thread == 0) {
        uint16_t data = -1;
        if (wanted_signals->motor_rpm == 1) {
            Modbus_read_registers(&targs->modbus, 1, MODBUS_RPM_REGADDR, 1, &data);
            fjson_object_object_add(fj_obj_motor_rpm, "value", fjson_object_new_double((double)data));
        }

        if (wanted_signals->brake_1_voltage == 1) {
            Modbus_read_registers(&targs->modbus, 1, MODBUS_BRAKE_1_VOLTAGE_REGADDR, 1, &data);
            fjson_object_object_add(fj_obj_brake_1_voltage, "value", fjson_object_new_double((double)data / 4096.));
        }

        if (wanted_signals->brake_2_voltage == 1) {
            Modbus_read_registers(&targs->modbus, 1, MODBUS_BRAKE_2_VOLTAGE_REGADDR, 1, &data);
            fjson_object_object_add(fj_obj_brake_2_voltage, "value", fjson_object_new_double((double)data / 4096.));
        }

        if (wanted_signals->throttle_voltage == 1) {
            Modbus_read_registers(&targs->modbus, 1, MODBUS_THROTTLE_VOLTAGE_REGADDR, 1, &data);
            fjson_object_object_add(fj_obj_throttle_voltage, "value", fjson_object_new_double((double)data / 4096.));
        }

        JSONInterface_dump_if_needed(6, 1);
        usleep(g_task_sleep_time);
    }
}

int ModbusReceiver_start()
{
    if (pthread_create(&thread, NULL, thread_loop, (void*)&targs)){
        fprintf(stderr, "Error creating modbus thread\n");
        return -1;
    }

    return 0;
}

void ModbusReceiver_stop()
{
    end_thread = 1;
    pthread_join(thread, NULL);
    Modbus_deinit(&targs.modbus);
}
