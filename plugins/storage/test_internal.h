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
 */

/****************************************************************************
 * \project Decentralized Access Control
 * \file test_internal.h
 * \brief
 * Header file with definition of test parameters for test_policyStore.c
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 04.09.2018. Initial version.
 ****************************************************************************/
//FIXME: Is this file in use? If not, it should be removed

#define POL_LEN 1224

char demo_test[] = {""};

char lock_Minh[] 			= {0x53, 0x15, 0x65, 0xB8, 0xF4, 0x0E, 0xCA, 0x5A, 0xAC, 0x55, 0xFE, 0xE6, 0x66, 0x4A, 0x23, 0x3E, 0x2D, 0x03, 0x12, 0x85, 0x6B, 0x77, 0x7D, 0x87, 0x5E, 0xB9, 0xAC, 0xC4, 0xF8, 0x5D, 0x1E, 0x99};

char unlock_Minh[] 			= {0x45, 0xC8, 0x85, 0x99, 0x13, 0x11, 0x56, 0xAE, 0xD1, 0x6B, 0x48, 0xFB, 0x6A, 0x57, 0xB8, 0x21, 0x1F, 0x24, 0xB9, 0x28, 0x77, 0x07, 0x61, 0x64, 0xAC, 0x5D, 0x14, 0x3A, 0x81, 0x73, 0x06, 0x45};

char honk_Minh[] 			= {0x50, 0x86, 0x7C, 0xC9, 0x32, 0x54, 0x92, 0x04, 0x92, 0xFC, 0xE0, 0xBF, 0x8D, 0xAD, 0xEA, 0x3A, 0xD8, 0xC7, 0xC8, 0x91, 0xA6, 0x3B, 0x83, 0x09, 0xE4, 0xA9, 0x28, 0xAF, 0x5D, 0x2C, 0xE2, 0xA0};

char alarm_on_Minh[]       	= {0x34, 0xF3, 0xC3, 0x1C, 0x66, 0xD7, 0xE9, 0xE7, 0x58, 0x6E, 0xAA, 0XD8, 0x9C, 0xC8, 0x58, 0x67, 0x68, 0x97, 0x33, 0x88, 0x59, 0x93, 0x83, 0x85, 0xF5, 0xDB, 0x56, 0x05, 0x26, 0x60, 0x44, 0xA1};

char alarm_off_Minh[]    	= {0x34, 0xF3, 0xC3, 0x1C, 0x66, 0xD7, 0xE9, 0xE7, 0x58, 0x6E, 0xAA, 0XD8, 0x9C, 0xC8, 0x58, 0x67, 0x68, 0x97, 0x33, 0x88, 0x59, 0x93, 0x83, 0x85, 0xF5, 0xDB, 0x56, 0x05, 0x26, 0x60, 0x44, 0xA2};

char demo_pol1[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol2[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol3[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol4[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol5[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol6[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol7[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol8[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol9[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char demo_pol0[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char policy_1[POL_LEN] = {0};
char policy_2[POL_LEN] = {0};
char policy_3[POL_LEN] = {0};
char policy_4[POL_LEN] = {0};
char policy_5[POL_LEN] = {0};
char policy_6[POL_LEN] = {0};
char policy_7[POL_LEN] = {0};
char policy_8[POL_LEN] = {0};
char policy_9[POL_LEN] = {0};
char policy_0[POL_LEN] = {0};

char* demo_pol_store[] = {demo_pol1, demo_pol2, demo_pol3, demo_pol4, demo_pol5, demo_pol6, demo_pol7, demo_pol8, demo_pol9, demo_pol0};

static int pol_store_counter = 0;

char* pol_store[] = {policy_1, policy_2, policy_3, policy_4, policy_5, policy_6, policy_7, policy_8, policy_9, policy_0};
