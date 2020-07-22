/*
 * This file is part of the IOTA Access distribution
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

/***************************************************************************
 * Project:     IOTA Access
 *
 * Module:      Policy validation tool
 *
 * Info:        This module is created in order to validate or
 *              discard policy, also it has functionality to
 *              perform policy optimisation.
 *
 * File:        validator.h
 *
 * Designed-by: Djordje Golubovic
 *
 * History:     21.02.2020. - Initial version
 *              22.03.2020. - Value limitations check
 *              25.03.2020. - Moved parser enum to helper module
 *****************************************************************************/
#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

/***************************************************************************
 * DEFINES
 ****************************************************************************/
#define VALIDATOR_MAX_TOKENS 256
#ifndef bool
#define bool _Bool
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/***************************************************************************
 * ENUMERATIONS
 ****************************************************************************/

typedef enum {
  FL_HAS_NONE = 0x00,
  FL_HAS_COST = 0x01,
  FL_HAS_HASH = 0x02,
  FL_HAS_POLID = 0x04,
  FL_HAS_POLOB = 0x08,
  FL_HAS_ALL = 0x0F
} validator_policy_first_level_e;

typedef enum {
  DG_HAS_NONE = 0x00,
  DG_HAS_DOC = 0x01,
  DG_HAS_GOC = 0x02,
  DG_HAS_ATTLIST = 0x04,
  DG_HAS_OPP = 0x08,
  DG_HAS_ALL = 0x0F
} validator_policy_docgoc_level_e;

typedef enum { CT_INT, CT_FLOAT, CT_HASH_FN, CT_ID, CT_TIME } validator_cmp_types_e;

typedef enum { CR_FALSE, CR_TRUE, CR_NOT_SUPPORTED, CR_BAD_ARG } validator_check_res_e;

/***************************************************************************
 * STRUCTURES
 ****************************************************************************/
typedef struct {
  int valid_json;
  int proper_format;
  int no_dead_code;
  int boundaries_met;
  int suspicious_conditions;
} validator_report_t;

/***************************************************************************
 * FUNCTION DECLARATIONS
 ****************************************************************************/
void validator_check(const char*, validator_report_t*);

#endif
