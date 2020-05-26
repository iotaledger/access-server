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
 * \file storage.h
 * \brief
 * Implementation of policy storage interface
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 *
 * \history
 * 24.08.2018. Initial version.
 * 25.05.2020. Refactoring.
 ****************************************************************************/
#ifndef _STORAGE_H_
#define _STORAGE_H_

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

//Set R-Pi as used platform
#define USE_RPI 1

/****************************************************************************
 * TYPES
 ****************************************************************************/
typedef enum
{
    STORAGE_OK,
    STORAGE_ERROR,
} STORAGE_error_t;

/****************************************************************************
 * API FUNCTIONS
 ****************************************************************************/
/**
 * @fn      Storage_init
 *
 * @brief   Initialize storage module
 * 
 * @param   void
 *
 * @return  STORAGE_error_t state.
 */
STORAGE_error_t Storage_init(void);

/**
 * @fn      Storage_term
 *
 * @brief   Terminate storage module
 * 
 * @param   void
 *
 * @return  STORAGE_error_t state.
 */
STORAGE_error_t Storage_term(void);


#endif //_STORAGE_H_
