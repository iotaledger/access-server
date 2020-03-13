/***************************************************************************
 * Project:     Remote Access Delegation
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
 *****************************************************************************/
#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

/***************************************************************************
 * DEFINES
****************************************************************************/
#define VALIDATOR_MAX_TOKENS 256
#define TRUE 1
#define FALSE 0

/***************************************************************************
 * ENUMERATIONS
****************************************************************************/
typedef _Bool bool;

typedef enum
{
    FL_HAS_NONE = 0x00,
    FL_HAS_COST = 0x01,
    FL_HAS_HASH = 0x02,
    FL_HAS_POLID = 0x04,
    FL_HAS_POLOB = 0x08,
    FL_HAS_ALL = 0x0F
} Validator_policy_first_level_e;

typedef enum
{
    DG_HAS_NONE = 0x00,
    DG_HAS_DOC = 0x01,
    DG_HAS_GOC = 0x02,
    DG_HAS_ATTLIST = 0x04,
    DG_HAS_OPP = 0x08,
    DG_HAS_ALL = 0x0F
} Validator_policy_docgoc_level_e;

/***************************************************************************
 * STRUCTURES
****************************************************************************/
typedef struct
{
    int valid_json;
    int proper_format;
    int no_dead_code;
    int boundaries_met;
    int suspicious_conditions;
} Validator_report_t;

/***************************************************************************
 * FUNCTION DECLARATIONS
****************************************************************************/
void Validator_check(const char*, Validator_report_t*);

#endif
