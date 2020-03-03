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
 * \file vehicle_dataset.c
 * \brief
 * Vehicle dataset interface implementation
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 26.06.2019. Initial version.
 ****************************************************************************/

#include "vehicle_dataset.h"

#include <stdlib.h>
#include <libfastjson/json.h>
#include <string.h>
#include "jsmn.h"

char VehicleDatasetCan01_options[] =
    "DoorDrvrReSts"
    "|DoorDrvrSts"
    "|VehMtnStSafe"
    "|EngCooltLvl"
    "|TrSts"
    "|DoorPassSts"
    "|DoorPassReSts"
    "|LockgCenStsForUsrFb"
    "|VehSpdLgtSafe"
    "|FuLvlIndcdVal"
    "|EngN"
    "|BkpOfDstTrvld"
    "|AmbTIndcd"
    "|VehSpdIndcdVal"
    "|VinBlk"
    "|VinPosn1"
    "|VinPosn2"
    "|VinPosn3"
    "|VinPosn4"
    "|VinPosn5"
    "|VinPosn6"
    "|VinPosn7"
    "|TrLockSts"
    "|VehBattUSysU"
    "|AmbTValAtDrvrSide"
    "|AmbTURawAtLeSideForObd"
    "|AmbTURawAtRiSideForObd"
    "|AmbTValAtPassSide"
    "|DrvrBrkTqAtWhlsReqd"
    "|BrkPedlPsd"
    "|BrkTqTotReqForPt"
    "|GearIndcn"
    "|DrvrPrpsnTqReq"
    "|CluPedlRat";

char VehicleDatasetCanopen01_options[] =
    "bms_bat_voltage"
    "|analogue_brake_full_voltage"
    "|analogue_brake_off_voltage"
    "|controller_temperature"
    "|vehicle_speed"
    "|motor_rpm"
    "|motor_speed"
    "|battery_voltage"
    "|battery_current"
    "|battery_soc"
    "|throttle_voltage"
    "|brake_1_voltage"
    "|brake_2_voltage"
    "|battery_power_percent"
    "|raw_battery_voltage"
    "|wheel_rpm_speed_sensor_based"
    "|wheel_rpm_motor_based"
    "|trip_meter"
    "|remote_throttle_voltage";

static int set_token(VehicleDataset_state_t *state, const char* name, int val)
{
    for (int i=0; i<state->options_count; i++)
    {
        if (strncmp(state->tokens[i].name, name, strlen(name)) == 0) {
            state->tokens[i].val = val;
            return 0;
        }
    }
    // index not found
    return -1;
}

void VehicleDataset_init(VehicleDataset_state_t *state) {
    unsigned char* dataset = (unsigned char*)state->dataset;
    if (state->options_count != 0) {
        for (int i=0; i<state->options_count; i++) {
            dataset[i] = 0;
            state->tokens[i].val = 0;
        }
    } else {
        // tokenize
        char* tok;
        // count tokens
        tok = &state->options[0];
        int vd_options_count_tmp = 1;
        while (*tok != '\0'){
            if (*tok == '|') vd_options_count_tmp++;
            tok++;
        }
        if (vd_options_count_tmp != 1) {
            state->options_count = vd_options_count_tmp;
            state->tokens = (vd_token_t*)malloc(state->options_count*sizeof(vd_token_t));

            tok = strtok(state->options, "|");
            for (int i=0; i<state->options_count; i++)
            {
                state->tokens[i].name = tok;
                state->tokens[i].val = 0;
                tok = strtok(NULL, "|");
                dataset[i] = 0;
            }
        }
    }
}

void VehicleDataset_deinit(VehicleDataset_state_t *state)
{
    free(state->dataset);
    free(state->tokens);
}

void VehicleDataset_from_json(VehicleDataset_state_t *state, const char* json, size_t json_len)
{
    int num_of_tokens = -1;
    jsmn_parser parser;
    jsmntok_t tokens[256];
    int arr_len = 0;
    int arr_index = -1;

    jsmn_init(&parser);
    num_of_tokens = jsmn_parse(&parser, json, json_len, NULL, 0);

    if ((num_of_tokens == -1) || (num_of_tokens < 1))
        return;

    jsmn_init(&parser);
    num_of_tokens = jsmn_parse(&parser, json, json_len, (jsmntok_t *)&tokens, num_of_tokens);

    for (int i = 0; i < num_of_tokens; i++)
    {
        if (tokens[i].type == JSMN_ARRAY)
        {
            arr_index = i;
            break;
        }
    }

    if (arr_index == -1)
        return;

    arr_len = tokens[arr_index].size;

    VehicleDataset_init(state);

    if (arr_len == 0)
        return;

    for (int i = 0, arr_start = arr_index + 1; i < arr_len; i++)
    {
        char tokname[128];
        int toklen = tokens[arr_start + i].end - tokens[arr_start + i].start;
        strncpy(tokname, json + tokens[arr_start + i].start, toklen);
        tokname[toklen] = '\0';
        set_token(state, tokname, 1);
    }

    unsigned char *dataset_uint8 = (unsigned char*)state->dataset;

    for (int i=0; i<state->options_count; i++)
    {
        dataset_uint8[i] = state->tokens[i].val;
    }
}

int VehicleDataset_to_json(VehicleDataset_state_t *state, char* json)
{
    fjson_object* arr = fjson_object_new_array();
    unsigned char *dataset_uint8 = (unsigned char*)state->dataset;

    for (int i=0; i<state->options_count; i++){
        if (dataset_uint8[i] == 1) {
            fjson_object_array_add(arr, fjson_object_new_string(state->tokens[i].name));
        }
    }
    const char* json_string = fjson_object_to_json_string(arr);
    int retval = strlen(json_string);
    strcpy(json, json_string);
    return retval;
}

int VehicleDataset_checked_count(VehicleDataset_state_t *state)
{
    unsigned char *vd_options_uint8 = (unsigned char*)state->dataset;
    int count = 0;
    for (int i=0; i<state->options_count; i++)
    {
        count += vd_options_uint8[i];
    }
    return count;
}
