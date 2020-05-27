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
 * Header file with definition of test API for storage plugin
 *
 * @Author Dejan Nedic, Strahinja Golic
 *
 * \notes
 * Plugins and PEP module must be initialized before test run
 *
 * \history
 * 04.09.2018. Initial version.
 * 27.05.2020. Refactor
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "pap.h"

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

#define TEST_PUBLIC_KEY_LEN 32
#define TEST_PRIVATE_KEY_LEN 64
#define TEST_SIGNATURE_LEN 64
#define TEST_POL_ID_MAX_LEN 64

#if PAP_STORAGE_TEST_ACIVE
#define TEST_POLICY_STORAGE(X) {if (!TEST_policy_storage()) return X;}
#else
#define TEST_POLICY_STORAGE(X)
#endif

/****************************************************************************
 * TEST API
 ****************************************************************************/
/**
 * @fn      TEST_policy_storage
 *
 * @brief   Test policy store-get-delete cycle
 *
 * @param   void
 *
 * @return  TRUE - test passed, FALSE - test fail
 */
bool TEST_policy_storage();