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
 * \file pep.h
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 * 11.05.2020. Refactoring
 ****************************************************************************/
#ifndef PEP_H
#define PEP_H

/****************************************************************************
 * MACROS
 ****************************************************************************/
#ifndef bool
#define bool _Bool
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define PEP_MAX_ACT_CALLBACKS 5  // high number, allow for unused callback slots

/****************************************************************************
 * ENUMERATIONS
 ****************************************************************************/
typedef enum { PEP_NO_ERROR, PEP_ERROR } pep_error_e;

// Note: this should not be here since PEP is platform agnostic
/*
typedef enum {
    PEP_CAN = 0,
    PEP_RELAY = 1,
    PEP_WALLET = 2,
} pep_actuators_e;
*/

/****************************************************************************
 * CALLBACKS
 ****************************************************************************/
typedef bool (*resolver_fn)(char* obligation, void* action);

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/

/**
 * @fn      pep_init
 *
 * @brief   Initialize module
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pep_init(void);

/**
 * @fn      pep_term
 *
 * @brief   Terminate module
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pep_term(void);

/**
 * @fn      PEP_register_callback
 *
 * @brief   Register resolver callback
 *
 * @param   resolver - Resolver callback
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pep_register_callback(int actuator, resolver_fn resolver);

/**
 * @fn      pep_unregister_callback
 *
 * @brief   Unregister resolver callback
 *
 * @param   void
 *
 * @return  TRUE on success, FALSE on failure
 */
bool pep_unregister_callback(void);

/**
 * @fn      int pep_request_access(JSON_Value *request)
 *
 * @brief   Function that computes decision for PEP
 *
 * @param   request         request JSON containing request uri and request object
 * @param   response        response to action
 *
 * @return  Fail (0), Success (1)
 */
bool pep_request_access(char* request, void* response);

#endif
