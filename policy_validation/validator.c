#include "validator.h"

#include <string.h>

#define JSMN_HEADER
#include "jsmn.h"

void Validator_check(const char* policy_data, Validator_report_t* report) {
    jsmn_parser parser;
    jsmn_init(&parser);
    memset(report, 0, sizeof(Validator_report_t));

    jsmntok_t tokens[VALIDATOR_MAX_TOKENS];
    int n = jsmn_parse(&parser, policy_data, strlen(policy_data), tokens, VALIDATOR_MAX_TOKENS);

    if (n > 0) {
        report->valid_json = 1;
    }
}
