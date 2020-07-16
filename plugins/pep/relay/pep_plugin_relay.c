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
 * \file pep_plugin_relay.c
 * \brief
 * PEP plugin for relay.
 *
 * @Author Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 12.07.2020. Initial version.
 ****************************************************************************/

#include "pep_plugin_relay.h"

#include <string.h>
#include <unistd.h>

#include "config_manager.h"
#include "datadumper.h"
#include "dlog.h"
#include "relay_interface.h"
#include "time_manager.h"
#include "wallet.h"

#define RES_BUFF_LEN 80
#define MAX_ACTIONS 10
#define ACTION_NAME_SIZE 16
#define POLICY_ID_SIZE 64
#define ADDR_SIZE 128

// wallet parameters for obligation of logging action on Tangle
#define NODE_URL "nodes.comnet.thetangle.org"
#define NODE_PORT 443
#define NODE_DEPTH 3
#define NODE_MWM 14
#define WALLET_SEED "DEJUXV9ZQMIEXTWJJHJPLAWMOEKGAYDNALKSMCLG9APR9LCKHMLNZVCRFNFEPMGOBOYYIKJNYWSAKVPAI"
#define ADDR "MXHYKULAXKWBY9JCNVPVSOSZHMBDJRWTTXZCTKHLHKSJARDADHJSTCKVQODBVWCYDNGWFGWVTUVENB9UA"
static char const *amazon_ca1_pem =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n"
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n"
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n"
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n"
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n"
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n"
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n"
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n"
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n"
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n"
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n"
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n"
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n"
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n"
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n"
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n"
    "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"
    "-----END CERTIFICATE-----\r\n";

typedef int (*action_t)(pdp_action_t* action, int should_log);

typedef struct {
  char action_names[MAX_ACTIONS][ACTION_NAME_SIZE];
  action_t actions[MAX_ACTIONS];
  size_t count;
} action_set_t;

static wallet_ctx_t* dev_wallet = NULL;
static action_set_t g_action_set;

static int log_tangle(){
  char bundle[81];
  wallet_send(dev_wallet, ADDR, 0, NULL, bundle);
}

static int relay_on()
{
  relayinterface_on(0);
  return 0;
}

static int relay_off()
{
  relayinterface_off(0);
  return 0;
}

static int destroy_cb(plugin_t* plugin, void* data) { free(plugin->callbacks); }

static int action_cb(plugin_t* plugin, void* data) {
  pep_plugin_args_t* args = (pep_plugin_args_t*)data;
  pdp_action_t* action = &args->action;
  char* obligation = args->obligation;
  bool should_log = FALSE;
  char buf[RES_BUFF_LEN];
  int status = 0;

  // handle obligations
  if (0 == memcmp(obligation, "obligation#1", strlen("obligation#1"))) {
    should_log = TRUE;
  }

  // execute action
  for (int i = 0; i < g_action_set.count; i++) {
    if (memcmp(action->value, g_action_set.action_names[i], strlen(g_action_set.action_names[i])) == 0) {
      timemanager_get_time_string(buf, RES_BUFF_LEN);
      dlog_printf("%s %s\t<Action performed>\n", buf, action->value);
      status = g_action_set.actions[i](action, should_log);
      break;
    }
  }
  return status;
}

int pep_plugin_relay_initializer(plugin_t* plugin, void* options) {
  dev_wallet = wallet_create(NODE_URL, NODE_PORT, amazon_ca1_pem, NODE_DEPTH, NODE_MWM, WALLET_SEED);

  g_action_set.actions[0] = relay_on;
  g_action_set.actions[1] = relay_off;
  strncpy(g_action_set.action_names[0], "action#1", ACTION_NAME_SIZE);
  strncpy(g_action_set.action_names[1], "action#2", ACTION_NAME_SIZE);
  g_action_set.count = 2;

  plugin->destroy = destroy_cb;
  plugin->callbacks = malloc(sizeof(void*) * PEP_PLUGIN_CALLBACK_COUNT);
  plugin->callbacks_num = PEP_PLUGIN_CALLBACK_COUNT;
  plugin->plugin_specific_data = NULL;
  plugin->callbacks[PEP_PLUGIN_ACTION_CB] = action_cb;

  return 0;
}
