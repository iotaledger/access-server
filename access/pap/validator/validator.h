#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#define VALIDATOR_MAX_TOKENS 256

typedef struct {
    int valid_json;
    int proper_format;
    int no_dead_code;
    int boundaries_met;
    int suspicious_conditions;

} Validator_report_t;

void Validator_check(const char*, Validator_report_t*);

#endif
