/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
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
* \file pip_plugin_modbus.c
* \brief
* Modbus receiver module interface
*
* @Author Djordje Golubovic, Bernardo Araujo
*
* \notes
*
* \history
* 07.29.2019. Initial version.
* 15.07.2020. Renaming.
 ****************************************************************************/

#include "pip_plugin_modbus.h"

#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"
#include "globals_declarations.h"

#define MODBUS_JSON_NAME "modbus_data"
#define MODBUS_BUFF_LEN 128
#define MODBUS_SERIAL_DEV_LEN 64
#define MODBUS_DATA_RESOLUTION 4096.
#define MODBUS_JSON_DUMP_PERIOD_S 6
#define MODBUS_BRAKE_1_VOLTAGE_REGADDR 0x010f
#define MODBUS_BRAKE_2_VOLTAGE_REGADDR 0x0110
#define MODBUS_RPM_REGADDR 0x107
#define MODBUS_THROTTLE_VOLTAGE_REGADDR 0x010e

typedef struct {
  char serial_device[MODBUS_SERIAL_DEV_LEN];
  fjson_object* fj_root;
  pthread_mutex_t* json_mutex;
  modbus_t modbus;
} modbusreceiver_thread_args_t;

static modbusreceiver_thread_args_t targs;
static canopen01_vehicle_dataset_t* wanted_signals;

// modbus data stuff
static fjson_object* fj_obj_modbus;
static fjson_object* fj_obj_motor_rpm;
static fjson_object* fj_obj_brake_1_voltage;
static fjson_object* fj_obj_brake_2_voltage;
static fjson_object* fj_obj_throttle_voltage;

static fjson_object* modbus_json_filler() {
  fj_obj_modbus = fjson_object_new_array();

  if (wanted_signals->motor_rpm == 1) {
    fj_obj_motor_rpm = fjson_object_new_object();
    fjson_object_object_add(fj_obj_motor_rpm, "name", fjson_object_new_string("motor_rpm"));
    fjson_object_array_add(fj_obj_modbus, fj_obj_motor_rpm);
  } else {
    fj_obj_motor_rpm = NULL;
  }

  if (wanted_signals->brake_1_voltage == 1) {
    fj_obj_brake_1_voltage = fjson_object_new_object();
    fjson_object_object_add(fj_obj_brake_1_voltage, "name", fjson_object_new_string("brake_1_voltage"));
    fjson_object_array_add(fj_obj_modbus, fj_obj_brake_1_voltage);
  } else {
    fj_obj_brake_1_voltage = NULL;
  }

  if (wanted_signals->brake_2_voltage == 1) {
    fj_obj_brake_2_voltage = fjson_object_new_object();
    fjson_object_object_add(fj_obj_brake_2_voltage, "name", fjson_object_new_string("brake_2_voltage"));
    fjson_object_array_add(fj_obj_modbus, fj_obj_brake_2_voltage);
  } else {
    fj_obj_brake_1_voltage = NULL;
  }

  if (wanted_signals->throttle_voltage == 1) {
    fj_obj_throttle_voltage = fjson_object_new_object();
    fjson_object_object_add(fj_obj_throttle_voltage, "name", fjson_object_new_string("throttle_voltage"));
    fjson_object_array_add(fj_obj_modbus, fj_obj_throttle_voltage);
  } else {
    fj_obj_throttle_voltage = NULL;
  }

  return fj_obj_modbus;
}

// !modbus data stuff

void modbusreceiver_init(canopen01_vehicle_dataset_t* dataset, pthread_mutex_t* json_mutex) {
  char buff[MODBUS_BUFF_LEN] = {0};
  config_manager_get_option_string("modbus", "serial_device", targs.serial_device, MODBUS_SERIAL_DEV_LEN);

  datadumper_add_module_init_cb(modbus_json_filler, &fj_obj_modbus, MODBUS_JSON_NAME);

  targs.json_mutex = json_mutex;
  wanted_signals = dataset;

  modbus_init(&targs.modbus, targs.serial_device);
}

static int end_thread = 0;
static pthread_t thread = 0;

static void* thread_loop(void* ptr) {
  modbusreceiver_thread_args_t* targs = (modbusreceiver_thread_args_t*)ptr;

  while (end_thread == 0) {
    uint16_t data = -1;
    if (wanted_signals->motor_rpm == 1) {
      modbus_read_registers(&targs->modbus, 1, MODBUS_RPM_REGADDR, 1, &data);
      fjson_object_object_add(fj_obj_motor_rpm, "value", fjson_object_new_double((double)data));
    }

    if (wanted_signals->brake_1_voltage == 1) {
      modbus_read_registers(&targs->modbus, 1, MODBUS_BRAKE_1_VOLTAGE_REGADDR, 1, &data);
      fjson_object_object_add(fj_obj_brake_1_voltage, "value",
                              fjson_object_new_double((double)data / MODBUS_DATA_RESOLUTION));
    }

    if (wanted_signals->brake_2_voltage == 1) {
      modbus_read_registers(&targs->modbus, 1, MODBUS_BRAKE_2_VOLTAGE_REGADDR, 1, &data);
      fjson_object_object_add(fj_obj_brake_2_voltage, "value",
                              fjson_object_new_double((double)data / MODBUS_DATA_RESOLUTION));
    }

    if (wanted_signals->throttle_voltage == 1) {
      modbus_read_registers(&targs->modbus, 1, MODBUS_THROTTLE_VOLTAGE_REGADDR, 1, &data);
      fjson_object_object_add(fj_obj_throttle_voltage, "value",
                              fjson_object_new_double((double)data / MODBUS_DATA_RESOLUTION));
    }

    datadumper_dump_if_needed(MODBUS_JSON_DUMP_PERIOD_S);
    usleep(g_task_sleep_time);
  }
}

int modbusreceiver_start() {
  if (pthread_create(&thread, NULL, thread_loop, (void*)&targs)) {
    fprintf(stderr, "Error creating modbus thread\n");
    return -1;
  }

  return 0;
}

void modbusreceiver_stop() {
  end_thread = 1;
  pthread_join(thread, NULL);
  modbus_deinit(&targs.modbus);
}
