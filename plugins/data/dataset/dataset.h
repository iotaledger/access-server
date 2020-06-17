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
 * \file dataset.h
 * \brief
 * Interface for different, platform depended, datasets
 *
 * @Author Strahinja Golic
 *
 * \notes
 * This file provides API prototypes, which must be implemented on specific
 * datasets.
 *
 * \history
 * 20.05.2020. Initial version.
 ****************************************************************************/
#ifndef __DATASET_H__
#define __DATASET_H__

/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdlib.h>

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define DATASET_ARRAY_SIZE 16

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct {
  char* name;
  int val;
} d_token_t;

typedef struct {
  char* options;
  int options_count;
  d_token_t* tokens;
  void* dataset;
} dataset_state_t;

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
extern char* dataset_options[DATASET_ARRAY_SIZE];

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
void dataset_init(dataset_state_t* state);
void dataset_deinit(dataset_state_t* state);
void dataset_from_json(dataset_state_t* state, const char* json, size_t json_len);
int dataset_to_json(dataset_state_t* state, char* json);
int dataset_checked_count(dataset_state_t* state);
#endif  //__DATASET_H__
