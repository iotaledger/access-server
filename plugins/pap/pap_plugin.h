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
 * \project IOTA Access
 * \file pap_plugin.h
 * \brief
 * Implementation of policy storage interface
 *
 * @Author Dejan Nedic, Strahinja Golic, Bernardo Araujo
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 25.05.2020. Refactoring.
 * 19.06.2020. Refactoring
 ****************************************************************************/
#ifndef _PAP_PLUGIN_H_
#define _PAP_PLUGIN_H_

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

// Set R-Pi as used platform
#define USE_RPI 1

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef enum {
  PAP_PLUGIN_OK,
  PAP_PLUGIN_ERROR,
} pap_plugin_error_t;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      pap_plugin_init
 *
 * @brief   Initialize storage module
 *
 * @param   void
 *
 * @return  pap_plugin_error_t state.
 */
pap_plugin_error_t pap_plugin_init(void);

/**
 * @fn      pap_plugin_term
 *
 * @brief   Terminate storage module
 *
 * @param   void
 *
 * @return  pap_plugin_error_t state.
 */
pap_plugin_error_t pap_plugin_term(void);

#endif  //_pap_plugin_H_
