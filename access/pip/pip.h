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
 * \file pip.h
 * \brief
 * Implementation of Policy Information Point
 *
 * @Author Milivoje Knezevic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 12.10.2018. Initial version.
 * 07.05.2020. Refactoring
 ****************************************************************************/
#ifndef _PIP_H_
#define _PIP_H_

#include "pdp.h"
#include "plugin.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PIP_MAX_STR_LEN 256
#define PIP_MAX_AUTH_CALLBACKS 1  // After each new authority is added, this must be incremented

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum { PIP_NO_ERROR, PIP_ERROR } pip_error_e;

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef struct {
  char type[PIP_MAX_STR_LEN];
  char value[PIP_MAX_STR_LEN];
} pip_attribute_object_t;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      pip_init
 *
 * @brief   Initialize module
 *
 * @param   void
 *
 * @return  pip_error_e error status
 */
pip_error_e pip_init(void);

/**
 * @fn      pip_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  pip_error_e error status
 */
pip_error_e pip_term(void);

pip_error_e pip_start(void);

pip_error_e pip_get_dataset(char* dataset_json, size_t* string_len);

pip_error_e pip_set_dataset(char* dataset_json, size_t string_len);

/**
 * @fn      pip_register_plugin
 *
 * @brief   Register callback for authority
 *
 * @param   authority - Authority which needs to register callback
 * @param   fetch - Callback to register
 *
 * @return  pip_error_e error status
 */
pip_error_e pip_register_plugin(plugin_t*);

/**
 * @fn      pip_get_data
 *
 * @brief   Get attributes from plugins
 *
 * @param   uri - Request to perform
 * @param   attribute - Fetched attribute
 *
 * @return  pip_error_e error status
 */
pip_error_e pip_get_data(char* uri, pip_attribute_object_t* attribute);

#endif /* _PIP_H_ */
