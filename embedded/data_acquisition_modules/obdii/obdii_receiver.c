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

#include "obdii_receiver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "OBDII.h"
#include "OBDIICommunication.h"
#include "json_interface.h"
#include <libfastjson/json.h>

static OBDIISocket s;
static OBDIICommandSet supportedCommands;

typedef struct {
    char portname[64];
    fjson_object* fj_root;
    pthread_mutex_t *json_mutex;
} ObdiiReceiver_thread_args_t;

static ObdiiReceiver_thread_args_t targs;

#define OBDII_JSON_NAME "obdii_data"

static fjson_object* fj_obj_obdii;
static fjson_object* obdii_json_filler()
{
    fj_obj_obdii = fjson_object_new_array();
    return fj_obj_obdii;
}

void ObdiiReceiver_init(const char* portname, pthread_mutex_t *json_mutex)
{
    if (OBDIIOpenSocket(&s, portname, 0x7E0, 0x7E8, 0) < 0) {
        printf("Error opening socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    supportedCommands = OBDIIGetSupportedCommands(&s);

    strncpy(targs.portname, portname, 64);

    JSONInterface_add_module_init_cb(obdii_json_filler, &fj_obj_obdii, OBDII_JSON_NAME);

    targs.json_mutex = json_mutex;
}

static int end_thread = 0;
static pthread_t obdii_thread = 0;

static void *obdii_thread_loop(void *ptr)
{
    while(end_thread == 0) {}
}

int ObdiiReceiver_start()
{
    if (pthread_create(&obdii_thread, NULL, obdii_thread_loop, (void*)&targs)){
        fprintf(stderr, "Error creating OBDII thread\n");
        return -2;
    }

    return 0;
}

void ObdiiReceiver_stop()
{
    end_thread = 1;
    OBDIICommandSetFree(&supportedCommands);
    OBDIICloseSocket(&s);
}
