/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \file pip_plugin_gpio.h
 * \brief
 * pip_plugin plugin for Raspberry Pi GPIO
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 19.06.2020. Initial version.
 ****************************************************************************/
#include "pip_plugin_gpio.h"
#include "datadumper.h"


static pip_plugin_t* g_probe_set = NULL;

static int gpio_pep_plugin_read(pip_plugin_probe_data_t *probe, int should_log)
{
    gpio_interface_read(0);
    return 0;
}

static void init_ds_interface(dataset_state_t* vdstate)
{

}

static void start_ds_interface()
{

}

static void stop_ds_interface()
{

}

static void term_ds_interface(dataset_state_t* vdstate)
{

}

void gpio_pip_plugin_initializer(pip_plugin_t* probe_set){
    if (probe_set == NULL)
    {
        return;
    }

    if (probe_set != NULL)
    {
        g_probe_set = probe_set;
    }

    g_probe_set->probes[0] = gpio_pep_plugin_read;
    strncpy(g_probe_set->probe_names[0], "read_gpio", PROBE_NAME_SIZE);
    g_probe_set->count = 1;
    g_probe_set->init_ds_interface_cb = init_ds_interface;
    g_probe_set->stop_ds_interface_cb = stop_ds_interface;
    g_probe_set->start_ds_interface_cb = start_ds_interface;
    g_probe_set->term_ds_interface_cb = term_ds_interface;
}

void gpio_pip_plugin_terminator()
{
    g_probe_set = NULL;
}
