/*
 * This file is part of the IOTA Access Distribution
 * (https://github.com/iotaledger/access)
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
 *
 * \file json_helper.c
 * \brief
 * JSON helper wrappers around jsmn JSON parser library
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 11.10.2018. Initial version.
 * 21.02.2020. API extension, solved bugs regarding token position.
 ****************************************************************************/
#include "json_helper.h"
#include <string.h>
#include <strings.h>
#include "dlog.h"

#define JSMN_HEADER
#include "jsmn.h"

#define GOBL_SIZE 16
#define DOBL_SIZE 15
#define TOK_MAX_SIZE 256

// const char array[] =
// {"{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"0\"},{\"type\":\"str\",\"value\":\"1\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array2[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array3[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array4[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array5[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array6[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array7[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array8[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array21[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array31[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array41[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array51[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array61[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array71[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

char array81[] = {
    "{\"obligation_deny\":{},\"obligation_grant\":{},\"policy_doc\":{\"type\":\"boolean\",\"value\":\"false\"},"
    "\"policy_goc\":{\"attribute_list\":[{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"},{\"type\":\"str\",\"value\":"
    "\"0x3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f"
    "1079a059f052ca6e51\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"},{\"type\":\"str\",\"value\":"
    "\"0xc73b8388fb1bd8f924af16308dba6615ff6aad1ae7e5d994016a35718ef039f6814e53df2cf5a22294d807aa43346a69c8bf229129b6fb"
    "93abb70cf3ad1277f0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":\"open_door\"},{"
    "\"type\":\"str\",\"value\":\"open_door\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"0\"},{\"type\":\"str\",\"value\":\"0\"}],\"operation\":\"eq\"},{\"attribute_list\":[{\"type\":\"str\",\"value\":"
    "\"3537973953\"},{\"type\":\"str\",\"value\":\"3537973953\"}],\"operation\":\"eq\"}],\"operation\":\"and\"}}"};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int r;

static jsmn_parser p;
static jsmntok_t t[TOK_MAX_SIZE];

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

int size_of_token(int position) {
  int ret = 0;

  if ((t[position].type == JSMN_STRING) || (t[position].type == JSMN_PRIMITIVE)) {
    ret += 1;
  } else if (t[position].type == JSMN_ARRAY) {
    int temp_pos = 1;
    for (int i = 0; i < t[position].size; i++) {
      temp_pos += size_of_token(position + temp_pos);
    }
    ret += temp_pos;
  } else if (t[position].type == JSMN_OBJECT) {
    int temp_pos = 1;
    for (int i = 0; i < t[position].size; i++) {
      temp_pos++;
      temp_pos += size_of_token(position + temp_pos);
    }

    ret += temp_pos;
  } else {
    // undefined
  }

  return ret;
}

int jsonhelper_get_value(const char *json, int position, const char *s) {
  int ret = JSON_ERROR;

  int token_size = t[position].size;

  int current = position + 1;

  for (int i = 0; i < token_size; i++) {
    if (jsoneq(json, &t[current], s) == 0) {
      //	dlog_printf("- UID: %.*s\n", tok[current + 1].end - tok[current
      //+ 1].start, array + tok[current + 1].start); 	dlog_printf("-
      // UID:
      //%.*s\n", tok[current].end - tok[current].start, array +
      // tok[current].start);
      ret = current + 1;

      break;
    }

    current++;
    current += size_of_token(current);
  }

  return ret;
}

int jsonhelper_parser_init(const char *policy) {
  int ret = 0;

  jsmn_init(&p);
  r = jsmn_parse(&p, policy, strlen(policy), t, sizeof(t) / sizeof(t[0]));
  //	dlog_printf("\nRL pi: (%d)", strlen(policy));

  if (r < 0) {
    dlog_printf("Failed to parse JSON: %d\n", r);
    return ret;
  }

  /* Assume the top-level element is an object */
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    dlog_printf("Object expected\n");
    return ret;
  }

  ret = r;

  return ret;
}

int jsonhelper_get_token_start(int token_id) { return t[token_id].start; }

int jsonhelper_get_token_end(int token_id) { return t[token_id].end; }

int jsonhelper_array_size(int token_id) {
  int ret = -1;

  if (t[token_id].type == JSMN_ARRAY) {
    ret = t[token_id].size;
  } else {
    // error not an array
  }

  return ret;
}

int jsonhelper_get_array_element(int token_id, int member_index) {
  int ret = -1;
  int i = 0;
  int temp_id = token_id + 1;

  if (t[token_id].type == JSMN_ARRAY) {
    if (member_index > t[token_id].size) {
      // error: should never happen
      return ret;
    }

    ret = token_id + 1;

    for (i = 0; i < member_index; i++) {
      while (t[ret].end >= t[temp_id].start) {
        temp_id++;
      }

      ret = temp_id;
    }
  }

  return ret;
}

int jsonhelper_get_attribute_from_array(int token_id, int index) {
  int ret = -1;
  int size = -1;

  if (t[token_id].type == JSMN_ARRAY) {
    size = t[token_id].size;
    if (index > size) {
      // error: should never happen
      return ret;
    }

    int temp_pos = 1;
    for (int i = 0; i < index; i++) {
      temp_pos += size_of_token(token_id + temp_pos);
    }

    ret = temp_pos;
  }

  return ret;
}

int jsonhelper_token_size(int token_id) { return t[token_id].end - t[token_id].start; }

int jsonhelper_has_token(const char *policy, const char *value, int size) {
  int ret = 0;

  for (int i = 0; i < r; i++) {
    if ((size == (t[i].end - t[i].start)) && (memcmp(policy + t[i].start, value, size) == 0)) {
      ret = 1;
      break;
    }
  }

  return ret;
}

int jsonhelper_get_value_with_type(const char *policy, const char *type, int type_size) {
  int ret = 0;

  for (int i = 0; i < r; i++) {
    if ((type_size == (t[i].end - t[i].start)) && (memcmp(policy + t[i].start, type, type_size) == 0)) {
      ret = i + 2;  // Skip 'value' token and return requested token
      break;
    }
  }

  return ret;
}

jsmntok_t jsonhelper_get_token_at(int i) { return t[i]; }

int jsonhelper_get_token_type(int token_id) { return t[token_id].type; }

int jsonhelper_get_action(char *action, char *policy, int number_of_tokens) {
  int ret = JSON_ERROR;

  for (int i = 0; i < number_of_tokens; i++) {
    /*if(jsoneq(json, &t[current], s) == 0)
    {
    //	dlog_printf("- UID: %.*s\n", tok[current + 1].end - tok[current
    + 1].start, array + tok[current + 1].start);
    //	dlog_printf("- UID: %.*s\n", tok[current].end -
    tok[current].start, array + tok[current].start); ret = current + 1;

            break;
    }

    current++;
    current += size_of_token(current);*/
    if ((t[i].end - t[i].start) == 6) {
      if ((strncasecmp(policy + t[i].start, "action", strlen("action")) == 0)) {
        memcpy(action, policy + t[i + 2].start, t[i + 2].end - t[i + 2].start);
        //				dlog_printf("ACTION IS:
        //%.*s\n",t[i
        //+ 2].end - t[i + 2].start,action);
        ret = t[i + 2].start;
        break;
      }
    }
  }

  return ret;
}

int jsonhelper_get_token_index(const char *json, const char *s) {
  int ret = JSON_ERROR;
  int tok_len;
  int tok_len_1;
  int tok_len_2;

  tok_len_1 = strlen(s);

  for (int i = 0; i < r; i++) {
    tok_len_2 = t[i].end - t[i].start;
    tok_len = tok_len_1 < tok_len_2 ? tok_len_1 : tok_len_2;

    if ((strncasecmp(json + t[i].start, s, tok_len) == 0) && (tok_len != 0)) {
      ret = i + 1;
      break;
    }
  }

  return ret;
}

int jsonhelper_get_token_index_from_pos(const char *json, int pos, const char *s) {
  int ret = JSON_ERROR;
  int tok_len;
  int tok_len_1;
  int tok_len_2;

  tok_len_1 = strlen(s);

  for (int i = pos; i < r; i++) {
    tok_len_2 = t[i].end - t[i].start;
    tok_len = tok_len_1 < tok_len_2 ? tok_len_1 : tok_len_2;

    if ((strncasecmp(json + t[i].start, s, tok_len) == 0) && (tok_len != 0)) {
      ret = i + 1;
      break;
    }
  }

  return ret;
}

int jsonhelper_get_token_num(void) { return r; }
