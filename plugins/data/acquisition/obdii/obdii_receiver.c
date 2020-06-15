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
 * \file obdii_receiver.c
 * \brief
 * OBDII interface implementation
 *
 * @Author
 *
 * \notes
 *
 * \history
 * XX.YY.ZZZZ. Initial version.
 ****************************************************************************/

#include "obdii_receiver.h"

#include <string.h>
#include <errno.h>
#include "OBDIICommunication.h"
#include "json_interface.h"

#define OBDII_NO_ERROR 0
#define OBDII_ERROR -2
#define OBDII_JSON_NAME "obdii_data"
#define OBDII_PORTNAME_LEN 64
#define OBDII_TX_ID 0x7E0
#define OBDII_RX_ID 0x7E8

typedef struct
{
    char portname[OBDII_PORTNAME_LEN];
    fjson_object* fj_root;
    pthread_mutex_t *json_mutex;
} ObdiiReceiver_thread_args_t;

static OBDIISocket s;
static OBDIICommandSet supportedCommands;
static ObdiiReceiver_thread_args_t targs;
static fjson_object* fj_obj_obdii;

static fjson_object* obdii_json_filler()
{
    fj_obj_obdii = fjson_object_new_array();
    return fj_obj_obdii;
}

void ObdiiReceiver_init(const char* portname, pthread_mutex_t *json_mutex)
{
    if (OBDIIOpenSocket(&s, portname, OBDII_TX_ID, OBDII_RX_ID, 0) < 0)
    {
        printf("Error opening socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    supportedCommands = OBDIIGetSupportedCommands(&s);

    strncpy(targs.portname, portname, OBDII_PORTNAME_LEN);

    JSONInterface_add_module_init_cb(obdii_json_filler, &fj_obj_obdii, OBDII_JSON_NAME);

    targs.json_mutex = json_mutex;
}

static int end_thread = 0;
static pthread_t obdii_thread = 0;

static void *obdii_thread_loop(void *ptr)
{
    while (end_thread == 0) {}
}

int ObdiiReceiver_start()
{
    if (pthread_create(&obdii_thread, NULL, obdii_thread_loop, (void*)&targs))
    {
        fprintf(stderr, "Error creating OBDII thread\n");
        return OBDII_ERROR;
    }

    return OBDII_NO_ERROR;
}

void ObdiiReceiver_stop()
{
    end_thread = 1;
    OBDIICommandSetFree(&supportedCommands);
    OBDIICloseSocket(&s);
}
