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
 * \file pip_plugin_can.h
 * \brief
 * PIP Plugin for CAN
 *
 * @Author Djordje Golubovic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 * 15.07.2020. Renaming.
 ****************************************************************************/

#include <string.h>

#include "pip_plugin_can.h"

#include "can_msgs.h"
#include "can_thread.h"
#include "config_manager.h"
#include "datadumper.h"
#include "vehicle_dataset.h"

#ifndef bool
#define bool _Bool
#endif

typedef enum { CAN_DOOR_UNKNOWN = 0x00, CAN_DOOR_OPENED = 0x01, CAN_DOOR_CLOSED = 0x02 } can_door_status_e;

typedef enum {
  CAN_LOCK_UNDEF = 0x00,
  CAN_LOCK_OPENED = 0x01,
  CAN_LOCK_CLOSED = 0x02,
  CAN_LOCK_LOCKED = 0x03,
  CAN_LOCK_SAFE = 0x04
} can_lock_status_e;

extern char vehicledatasetdemo01_options[];

typedef enum { CAN_TEMP_UNIT_C = 0x00, CAN_TEMP_UNIT_F = 0x01, CAN_TEMP_UNIT_UNKNOWN = 0x02 } can_temp_unit_e;

#define MAX_STR_SIZE 256
#define DATA_SIZE 8
#define CAN_JSON_NAME "can_data"
#define CAN_STR_LEN 1024
#define CAN_ID_MASK 0x7ff
#define JSON_DUMP_PERIOD_6S 6

static void be2le(unsigned char* in, unsigned char* out) {
  unsigned char data[DATA_SIZE];
  for (int i = 0; i < DATA_SIZE; i++) {
    data[i] = in[(DATA_SIZE - 1) - i];
  }
  for (int i = 0; i < DATA_SIZE; i++) {
    out[i] = data[i];
  }
}

static can01_vehicle_dataset_t* wanted_signals;
static bool is_in_use = FALSE;
static char body_chan[MAX_STR_SIZE];
static char chas_chan[MAX_STR_SIZE];

// CAN data stuff
static fjson_object* fj_obj_can;
static fjson_object* fj_values_array_ambient_air_temperature;
static fjson_object* fj_values_array_fuel_tank_level;
static fjson_object* fj_values_array_lock_status;
static fjson_object* fj_values_array_trunk_status;
static fjson_object* fj_values_array_driver_door_status;
static fjson_object* fj_values_array_driver_door_rear_status;
static fjson_object* fj_values_array_passenger_door_status;
static fjson_object* fj_values_array_passenger_door_rear_status;
static fjson_object* fj_values_array_requested_brake_torque_at_wheels;
static fjson_object* fj_values_array_requested_propulsion_torque;
static fjson_object* fj_values_array_clutch_pedal_position;
static fjson_object* fj_values_array_brake_pedal_pressed;

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

static fjson_object* can_json_filler() {
  fjson_object* fj_data_array = fjson_object_new_array();

  if (wanted_signals->AmbTIndcd == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(AmbTIndcd_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_ambient_air_temperature = fj_obj_tmp;
  } else {
    fj_values_array_ambient_air_temperature = NULL;
  }

  if (wanted_signals->FuLvlIndcdVal == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(FuLvlIndcdVal_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_fuel_tank_level = fj_obj_tmp;
  } else {
    fj_values_array_fuel_tank_level = NULL;
  }

  if (wanted_signals->LockgCenStsForUsrFb == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(LockgCenStsForUsrFb_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_lock_status = fj_obj_tmp;
  } else {
    fj_values_array_lock_status = NULL;
  }

  if (wanted_signals->TrSts == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(TrSts_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_trunk_status = fj_obj_tmp;
  } else {
    fj_values_array_trunk_status = NULL;
  }

  if (wanted_signals->DoorDrvrSts == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorDrvrSts_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_driver_door_status = fj_obj_tmp;
  } else {
    fj_values_array_driver_door_status = NULL;
  }

  if (wanted_signals->DoorDrvrReSts == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorDrvrReSts_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_driver_door_rear_status = fj_obj_tmp;
  } else {
    fj_values_array_driver_door_rear_status = NULL;
  }

  if (wanted_signals->DoorPassSts == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorPassSts_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_passenger_door_status = fj_obj_tmp;
  } else {
    fj_values_array_passenger_door_status = NULL;
  }

  if (wanted_signals->DoorPassReSts == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DoorPassReSts_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_passenger_door_rear_status = fj_obj_tmp;
  } else {
    fj_values_array_passenger_door_rear_status = NULL;
  }

  if (wanted_signals->DrvrBrkTqAtWhlsReqd == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DrvrBrkTqAtWhlsReqd_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_requested_brake_torque_at_wheels = fj_obj_tmp;
  } else {
    fj_values_array_requested_brake_torque_at_wheels = NULL;
  }

  if (wanted_signals->DrvrPrpsnTqReq == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(DrvrPrpsnTqReq_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_requested_propulsion_torque = fj_obj_tmp;
  } else {
    fj_values_array_requested_propulsion_torque = NULL;
  }

  if (wanted_signals->CluPedlRat == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(CluPedlRat_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_clutch_pedal_position = fj_obj_tmp;
  } else {
    fj_values_array_clutch_pedal_position = NULL;
  }

  if (wanted_signals->BrkPedlPsd == 1) {
    fjson_object* fj_obj_tmp = fjson_object_new_object();
    fjson_object_object_add(fj_obj_tmp, "name", fjson_object_new_string(BrkPedlPsd_JSON_NAME));
    fjson_object_array_add(fj_data_array, fj_obj_tmp);
    fj_values_array_brake_pedal_pressed = fj_obj_tmp;
  } else {
    fj_values_array_brake_pedal_pressed = NULL;
  }

  return fj_data_array;
}

// !CAN data stuff

void canreceiver_start();
int canreceiver_deinit();

static pthread_mutex_t* json_sync_lock;
static canthread_instance_t can_chas_instance;
static canthread_instance_t can_body_instance;
static void can_body_frame_read_cb(struct can_frame* frame);
static void can_chas_frame_read_cb(struct can_frame* frame);

static dataset_state_t ddstate;

void canreceiver_init(can01_vehicle_dataset_t* dataset, pthread_mutex_t* json_mutex) {
  wanted_signals = dataset;
  datadumper_add_module_init_cb(can_json_filler, &fj_obj_can, CAN_JSON_NAME);
  json_sync_lock = datadumper_get_mutex();
  config_manager_get_option_string("can_receiver", "can_body_channel", body_chan, MAX_STR_SIZE);
  config_manager_get_option_string("can_receiver", "can_chas_channel", chas_chan, MAX_STR_SIZE);

  canthread_init(&can_body_instance, body_chan, can_body_frame_read_cb);
  canthread_init(&can_chas_instance, chas_chan, can_chas_frame_read_cb);
  is_in_use = TRUE;
}

static int destroy_cb(plugin_t* plugin, void* data) {
  canreceiver_deinit();
  free(plugin->callbacks);
  return 0;
}

static int start_cb(plugin_t* plugin, void* data) {
  canreceiver_start();
  return 0;
}

static int set_dataset_cb(plugin_t* plugin, void* data) {
  pip_plugin_string_arg_t* arg = (pip_plugin_string_arg_t*)data;
  dataset_from_json(&ddstate, arg->string, arg->len);
  return 0;
}

static int get_dataset_cb(plugin_t* plugin, void* data) {
  pip_plugin_string_arg_t* arg = (pip_plugin_string_arg_t*)data;
  arg->len = dataset_to_json(&ddstate, arg->string);
  return 0;
}

int pip_plugin_can_initializer(plugin_t* plugin, void* data) {
  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PIP_PLUGIN_CALLBACK_COUNT);
  plugin->callbacks[PIP_PLUGIN_ACQUIRE_CB] = NULL;
  plugin->callbacks[PIP_PLUGIN_START_CB] = start_cb;
  plugin->callbacks[PIP_PLUGIN_GET_DATASET_CB] = get_dataset_cb;
  plugin->callbacks[PIP_PLUGIN_SET_DATASET_CB] = set_dataset_cb;
  plugin->callbacks_num = PIP_PLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;

  ddstate.options = &vehicledatasetdemo01_options[0];
  ddstate.dataset = malloc(sizeof(can01_vehicle_dataset_t));
  dataset_init(&ddstate);
  canreceiver_init(ddstate.dataset, NULL);
  return 0;
}

void canreceiver_start() {
  canthread_start(&can_body_instance);
  canthread_start(&can_chas_instance);
}

int canreceiver_deinit() {
  if (canthread_stop(&can_chas_instance)) return 1;
  if (canthread_stop(&can_body_instance)) return 2;
  if (ddstate.dataset != 0) {
    dataset_deinit(&ddstate);
  }
  is_in_use = FALSE;

  return 0;
}

bool canreceiver_is_in_use() { return is_in_use; }

void canreceiver_get_body_channel(char* channel_buff, int channel_buff_len) {
  if (channel_buff != NULL && channel_buff_len > 0) {
    memcpy(channel_buff, body_chan, channel_buff_len > MAX_STR_SIZE ? MAX_STR_SIZE : channel_buff_len);
  }
}

void canreceiver_get_chas_channel(char* channel_buff, int channel_buff_len) {
  if (channel_buff != NULL && channel_buff_len > 0) {
    memcpy(channel_buff, chas_chan, channel_buff_len > MAX_STR_SIZE ? MAX_STR_SIZE : channel_buff_len);
  }
}

static fjson_object* gen_interpreted_door_status(int status) {
  fjson_object* fj_value = fjson_object_new_object();
  fjson_object* fj_value_string;
  switch (status) {
    case CAN_DOOR_UNKNOWN:
      fj_value_string = fjson_object_new_string("Unknown");
      break;
    case CAN_DOOR_OPENED:
      fj_value_string = fjson_object_new_string("Opened");
      break;
    case CAN_DOOR_CLOSED:
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
    case CAN_LOCK_UNDEF:
      fj_value_string = fjson_object_new_string("Undefined");
      break;
    case CAN_LOCK_OPENED:
      fj_value_string = fjson_object_new_string("Opened");
      break;
    case CAN_LOCK_CLOSED:
      fj_value_string = fjson_object_new_string("Closed");
      break;
    case CAN_LOCK_LOCKED:
      fj_value_string = fjson_object_new_string("Locked");
      break;
    case CAN_LOCK_SAFE:
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
    case CAN_TEMP_UNIT_C:
      fj_value_string = fjson_object_new_string("C");
      break;
    case CAN_TEMP_UNIT_F:
      fj_value_string = fjson_object_new_string("F");
      break;
    case CAN_TEMP_UNIT_UNKNOWN:
      fj_value_string = fjson_object_new_string("Unknown unit");
      break;
    default:
      fj_value_string = fjson_object_new_string("ERROR");
  }
  fjson_object_object_add(fj_value, "value", fj_value_string);
  return fj_value;
}

static void can_body_frame_read_cb(struct can_frame* frame) {
  be2le(frame->data, frame->data);
#ifdef DEBUG_MODE
  char can_frame_string[CAN_STR_LEN];
  sprint_canframe(&can_frame_string[0], frame, 1);
  fprintf(stdout, "read can frame: %s\n", can_frame_string);
#endif
  unsigned canid = frame->can_id & CAN_ID_MASK;
  if (CanMsgs_body_msg_supported(canid)) {
    CanMsgs_BodyMessage_t bm;
    memcpy(&bm, frame->data, DATA_SIZE);

    pthread_mutex_lock(json_sync_lock);
    // CAN ID values are stated in can_msgs API
    switch (canid) {
      case 0x40:
        if (bm.msg_0x40.DoorDrvrReSts_UB == 1 && wanted_signals->DoorDrvrReSts == 1 &&
            fj_values_array_driver_door_rear_status != NULL) {
          fjson_object_object_add(fj_values_array_driver_door_rear_status, "value",
                                  gen_interpreted_door_status(bm.msg_0x40.DoorDrvrReSts));
        }
        if (bm.msg_0x40.DoorDrvrSts_UB == 1 && wanted_signals->DoorDrvrSts == 1 &&
            fj_values_array_driver_door_status != NULL) {
          fjson_object_object_add(fj_values_array_driver_door_status, "value",
                                  gen_interpreted_door_status(bm.msg_0x40.DoorDrvrSts));
        }
        break;
      case 0xE0:
        if (bm.msg_0xE0.TrSts_UB == 1 && wanted_signals->TrSts == 1 && fj_values_array_trunk_status != NULL) {
          fjson_object_object_add(fj_values_array_trunk_status, "value",
                                  gen_interpreted_door_status(bm.msg_0xE0.TrSts));
        }

        if (bm.msg_0xE0.DoorPassSts_UB == 1 && wanted_signals->DoorPassSts &&
            fj_values_array_passenger_door_status != NULL) {
          fjson_object_object_add(fj_values_array_passenger_door_status, "value",
                                  gen_interpreted_door_status(bm.msg_0xE0.DoorPassSts));
        }

        if (bm.msg_0xE0.DoorPassReSts_UB == 1 && wanted_signals->DoorPassReSts &&
            fj_values_array_passenger_door_rear_status != NULL) {
          fjson_object_object_add(fj_values_array_passenger_door_rear_status, "value",
                                  gen_interpreted_door_status(bm.msg_0xE0.DoorPassReSts));
        }
        break;
      case 0x100:
        if (bm.msg_0x100.LockgCenStsForUsrFb_UB == 1 && wanted_signals->LockgCenStsForUsrFb == 1 &&
            fj_values_array_lock_status != NULL) {
          fjson_object* fj_value = fjson_object_new_object();
          fjson_object_object_add(fj_value, "value", gen_interpreted_lock_status(bm.msg_0x100.LockgCenStsForUsrFb));
          fjson_object_object_add(fj_values_array_lock_status, "value", fj_value);
        }
        break;
      case 0x270:
        if (bm.msg_0x270.FuLvlIndcd_UB == 1 && wanted_signals->FuLvlIndcdVal == 1 &&
            fj_values_array_fuel_tank_level != NULL) {
          // minimum: 0, maximum: 204.6, factor: 0.2
          fjson_object* fj_value = fjson_object_new_object();
          fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)bm.msg_0x270.FuLvlIndcdVal * 0.2));
          fjson_object_object_add(fj_value, "unit", fjson_object_new_string("l"));
          fjson_object_object_add(fj_values_array_fuel_tank_level, "value", fj_value);
        }
      case 0x1D0:
        if (bm.msg_0x1D0.AmbTIndcdWithUnit_UB == 1 && wanted_signals->AmbTIndcd == 1 &&
            fj_values_array_ambient_air_temperature != NULL) {
          fjson_object* fj_value = fjson_object_new_object();
          fjson_object_object_add(fj_value, "value",
                                  fjson_object_new_double((double)bm.msg_0x1D0.AmbTIndcd * 0.1 - 100.));
          fjson_object_object_add(fj_value, "unit", gen_interpreted_temperature_unit(bm.msg_0x1D0.AmbTIndcdUnit));
          fjson_object_object_add(fj_values_array_ambient_air_temperature, "value", fj_value);
        }
        break;
    }
    pthread_mutex_unlock(json_sync_lock);
  }
  datadumper_dump_if_needed(JSON_DUMP_PERIOD_6S);
}

static void can_chas_frame_read_cb(struct can_frame* frame) {
  char can_frame_string[1024];
  be2le(frame->data, frame->data);
  sprint_canframe(&can_frame_string[0], frame, 1);
  unsigned canid = frame->can_id & 0x7ff;
  if (CanMsgs_chas_msg_supported(canid)) {
    CanMsgs_ChasMessage_t cm;
    memcpy(&cm, frame->data, 8);

    pthread_mutex_lock(json_sync_lock);
    switch (canid) {
      case 0xE0:
        if (cm.msg_0xE0.DrvrBrkTqAtWhlsReqdGroup_UB == 1 && wanted_signals->DrvrBrkTqAtWhlsReqd == 1 &&
            fj_values_array_requested_brake_torque_at_wheels != NULL) {
          fjson_object_object_add(fj_values_array_requested_brake_torque_at_wheels, "value",
                                  fjson_object_new_int(cm.msg_0xE0.DrvrBrkTqAtWhlsReqd));
        }
        break;
      case 0xF0:
        if (cm.msg_0xF0.TrSts_UB == 1 && wanted_signals->TrSts == 1 && fj_values_array_trunk_status != NULL) {
          fjson_object_object_add(fj_values_array_trunk_status, "value",
                                  gen_interpreted_door_status(cm.msg_0xF0.TrSts));
        }
        if (cm.msg_0xF0.DoorPassReSts_UB == 1 && wanted_signals->DoorPassReSts == 1 &&
            fj_values_array_passenger_door_rear_status != NULL) {
          fjson_object_object_add(fj_values_array_passenger_door_rear_status, "value",
                                  gen_interpreted_door_status(cm.msg_0xF0.DoorPassReSts));
        }
        if (cm.msg_0xF0.DoorDrvrSts_UB == 1 && wanted_signals->DoorDrvrSts == 1 &&
            fj_values_array_driver_door_status != NULL) {
          fjson_object_object_add(fj_values_array_driver_door_status, "value",
                                  gen_interpreted_door_status(cm.msg_0xF0.DoorDrvrSts));
        }
        if (cm.msg_0xF0.DoorPassSts_UB == 1 && wanted_signals->DoorPassSts == 1 &&
            fj_values_array_passenger_door_status != NULL) {
          fjson_object_object_add(fj_values_array_passenger_door_status, "value",
                                  gen_interpreted_door_status(cm.msg_0xF0.DoorPassSts));
        }
        break;
      case 0x3A0:
        if (cm.msg_0x3A0.DoorDrvrReSts_UB == 1 && wanted_signals->DoorDrvrReSts == 1 &&
            fj_values_array_driver_door_rear_status != NULL) {
          fjson_object_object_add(fj_values_array_driver_door_rear_status, "value",
                                  gen_interpreted_door_status(cm.msg_0x3A0.DoorDrvrReSts));
        }

        if (cm.msg_0x3A0.AmbTIndcdWithUnit_UB == 1 && wanted_signals->AmbTIndcd == 1 &&
            fj_values_array_ambient_air_temperature != NULL) {
          // minimum: -100, maximum: 309.5, factor: 0.1, offset: 100
          fjson_object* fj_value = fjson_object_new_object();
          fjson_object_object_add(fj_value, "value",
                                  fjson_object_new_double((double)cm.msg_0x3A0.AmbTIndcd * 0.1 - 100.));
          fjson_object_object_add(fj_value, "unit", gen_interpreted_temperature_unit(cm.msg_0x3A0.AmbTIndcdUnit));
          fjson_object_object_add(fj_values_array_ambient_air_temperature, "value", fj_value);
        }
        break;
      case 0x1C0:
        if (cm.msg_0x1C0.BrkPedlPsdSafeGroup_UB == 1 && wanted_signals->BrkPedlPsd == 1 &&
            fj_values_array_brake_pedal_pressed != NULL) {
          fjson_object_object_add(fj_values_array_brake_pedal_pressed, "value",
                                  fjson_object_new_int(cm.msg_0x1C0.BrkPedlPsd));
        }
        break;
      case 0x240:
        if (cm.msg_0x240.FuLvlIndcd_UB == 1 && wanted_signals->FuLvlIndcdVal == 1 &&
            fj_values_array_fuel_tank_level != NULL) {
          // minimum: 0, maximum: 204.6, factor: 0.2
          fjson_object* fj_value = fjson_object_new_object();
          fjson_object_object_add(fj_value, "value", fjson_object_new_double((double)cm.msg_0x240.FuLvlIndcdVal * 0.2));
          fjson_object_object_add(fj_value, "unit", fjson_object_new_string("l"));
          fjson_object_object_add(fj_values_array_fuel_tank_level, "value", fj_value);
        }
        break;
      case 0x130:
        if (cm.msg_0x130.DrvrPrpsnTqReq_UB == 1 && wanted_signals->DrvrPrpsnTqReq == 1 &&
            fj_values_array_requested_propulsion_torque != NULL) {
          fjson_object_object_add(fj_values_array_requested_propulsion_torque, "value",
                                  fjson_object_new_int(cm.msg_0x130.DrvrPrpsnTqReq));
        }
        break;
      case 0x210:
        if (cm.msg_0x210.CluPedlRat_UB == 1 && wanted_signals->CluPedlRat == 1 &&
            fj_values_array_clutch_pedal_position != NULL) {
          fjson_object_object_add(fj_values_array_clutch_pedal_position, "value",
                                  fjson_object_new_double((double)cm.msg_0x210.CluPedlRat * 0.392156862745));
        }
        break;
    }
    pthread_mutex_unlock(json_sync_lock);
  }
  datadumper_dump_if_needed(JSON_DUMP_PERIOD_6S);
}
