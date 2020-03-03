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
 * \project Policy Store
 * \file pep_internal.h
 * \brief
 * Internal and test values for pep
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 ****************************************************************************/
/*
unsigned char test_policy_hash_2[] = {0xc9, 0x5d, 0x35, 0xc9, 0x30, 0x1a, 0xeb, 0x8c, 0xad, 0x7f, 0x02, 0x7a, 0x39, 0x28, 0x5e, 0xbb, 0xd0, 0xd7, 0x0b, 0x65, 0xc3, 0xd7, 0x14, 0xd7, 0x64, 0x21, 0x18, 0x82, 0x7a, 0xe6, 0x27, 0xfb};

unsigned char test_policy_hash_3[] = {0xc2, 0x5d, 0x35, 0xc9, 0x30, 0x1a, 0xeb, 0x8c, 0xad, 0x7f, 0x02, 0x7a, 0x39, 0x28, 0x5e, 0xbb, 0xd0, 0xd7, 0x0b, 0x65, 0xc3, 0xd7, 0x14, 0xd7, 0x64, 0x21, 0x18, 0x82, 0x7a, 0xe6, 0x27, 0xfb};

char s1[] = { 0xdf, 0xee, 0xb1, 0x67, 0x27, 0xb4, 0x7c, 0xe6, 0xf2, 0x5e, 0x6d, 0x65, 0x1f, 0xa5, 0x88, 0xf9, 0x5d, 0xb1, 0x90, 0xdd, 0x09, 0xa8, 0x72, 0xd0, 0x5b, 0xd0, 0x58, 0x76, 0xca, 0xee, 0xc4, 0x2d};

char s2[] = { 0x35, 0x40, 0x87, 0x9a, 0x53, 0x6a, 0x3e, 0xef, 0x34, 0xb7, 0xb5, 0x41, 0xef, 0xeb, 0x1d, 0x2b, 0x1e, 0x4e, 0x72, 0x01, 0x22, 0x71, 0xe2, 0xdd, 0x1d, 0x6c, 0x1b, 0x3c, 0x40, 0x4e, 0x43, 0x67};

char s3[] = { 0xf4, 0xc8, 0x6b, 0xc8, 0x13, 0xe0, 0xac, 0x9e, 0x4c, 0x23, 0x48, 0x37, 0xdc, 0xfc, 0xb1, 0xe2, 0xaf, 0x43, 0xe2, 0x3e, 0xdf, 0x12, 0x2d, 0x9b, 0x51, 0xfb, 0x8b, 0xa7, 0x56, 0xba, 0x3e, 0x77};

char s4[] = { 0x95, 0x19, 0xe5, 0xcd, 0xb3, 0xc2, 0x06, 0x0c, 0x8d, 0xeb, 0xe5, 0x2f, 0x26, 0xa7, 0xcc, 0xea, 0x86, 0x7b, 0xb7, 0x93, 0x6b, 0xc7, 0x03, 0xeb, 0x16, 0x77, 0xb1, 0xbf, 0xf6, 0x15, 0x24, 0x5e};

char s5[] = { 0x69, 0x91, 0xce, 0x95, 0xb9, 0x5d, 0x20, 0xfe, 0x30, 0xd2, 0x1a, 0x76, 0xd8, 0x3f, 0x85, 0x4d, 0xb5, 0x0e, 0x57, 0xa2, 0x53, 0x1d, 0x1d, 0xe0, 0xfb, 0xc6, 0x96, 0x3e, 0xec, 0x71, 0x9b, 0xfc};

char s6[] = { 0x09, 0x37, 0xc6, 0xfc, 0xdf, 0x28, 0x30, 0xbc, 0xfb, 0xcc, 0x39, 0xa1, 0x3d, 0x7c, 0xd3, 0x73, 0x19, 0x69, 0xed, 0x55, 0xba, 0x56, 0xc5, 0x93, 0x93, 0x97, 0x61, 0x01, 0x3e, 0x47, 0x3f, 0xc8};

char s7[] = { 0xcf, 0x9d, 0x50, 0x93, 0x71, 0x6a, 0xb8, 0xd4, 0x29, 0x90, 0x51, 0x8a, 0xc5, 0xde, 0xc8, 0xce, 0x31, 0x5d, 0xd1, 0x5c, 0xf9, 0x10, 0x0f, 0xe6, 0x22, 0xaf, 0xfc, 0xe8, 0x38, 0x98, 0xd5, 0xd9};

char s8[] = { 0xa1, 0x85, 0xf5, 0x72, 0x0f, 0x09, 0xc9, 0x5d, 0x08, 0xfb, 0x76, 0xf2, 0x62, 0x42, 0xbf, 0x02, 0xb5, 0x9c, 0xc9, 0x54, 0xc4, 0x92, 0xc0, 0x13, 0x0c, 0x49, 0x58, 0x67, 0xee, 0xfe, 0x03, 0x89};
*/
char temp_policy_id[] = { 0xcf, 0x9d, 0x50, 0x93, 0x71, 0x6a, 0xb8, 0xd4, 0x29, 0x90, 0x51, 0x8a, 0xc5, 0xde, 0xc8, 0xce, 0x31, 0x5d, 0xd1, 0x5c, 0xf9, 0x10, 0x0f, 0xe6, 0x22, 0xaf, 0xfc, 0xe8, 0x38, 0x98, 0xd5, 0xd9};
