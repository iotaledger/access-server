#include "validator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSMN_HEADER
#include "jsmn.h"

char token_str[64];

void Validator_check(const char* policy_data, Validator_report_t* report) {
    jsmn_parser parser;
    jsmn_init(&parser);
    memset(report, 0, sizeof(Validator_report_t));

    jsmntok_t tokens[VALIDATOR_MAX_TOKENS];
    int n = jsmn_parse(&parser, policy_data, strlen(policy_data), tokens, VALIDATOR_MAX_TOKENS);

    if (n > 0) {
        report->valid_json = 1;
    } else {
        switch(n) {
            case JSMN_ERROR_NOMEM:
                printf("JSMN_ERROR_NOMEM\n");
            break;
            case JSMN_ERROR_INVAL:
                printf("JSMN_ERROR_INVAL\n");
            break;
            case JSMN_ERROR_PART:
                printf("JSMN_ERROR_PART\n");
            break;
        }
    }

    int proper_format = 1;
    // Top level token is key string "policy"
    if (strncmp(&policy_data[tokens[0].start], "policy", strlen("policy")) != 0 || tokens[0].size != 1) {
        proper_format = 0;
        goto proper_format_end;
    }

    // Value token of top level "policy" token is object
    if (tokens[1].type != JSMN_OBJECT || tokens[1].size != 4) {
        proper_format = 0;
        goto proper_format_end;
    }

    // Second level tokens
    int policy_object_idx = -1;
    int policy_object_end_idx = -1;
    int found_everyone = 0;
    for (int i = 2; i < n; i++) {
        if (tokens[i].size != 1) continue;
        // Check that token is a key, next token is value token and it is of correct type
        if (strncmp(&policy_data[tokens[i].start], "cost", strlen("cost")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].size == 0 &&
                tokens[i+1].type == JSMN_STRING) {
            // @TODO check if value token is in valid range, type, etc
            found_everyone |= 0x01;
            int next_idx = -1;
            for (int j=i+1; j<n; j++){
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx-1;
                    break;
                }
            }
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "hash_function", strlen("hash_function")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].size == 0 &&
                tokens[i+1].type == JSMN_STRING) {
            // @TODO check if value token is in valid range, type, etc
            found_everyone |= 0x02;
            int next_idx = -1;
            for (int j=i+1; j<n; j++){
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx-1;
                    break;
                }
            }
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_id", strlen("policy_id")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].size == 0 &&
                tokens[i+1].type == JSMN_STRING) {
            // @TODO check if value token is in valid range, type, etc
            found_everyone |= 0x04;
            int next_idx = -1;
            for (int j=i+1; j<n; j++){
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx-1;
                    break;
                }
            }
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_object", strlen("policy_object")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {
            found_everyone |= 0x08;
            policy_object_idx = i;
            int next_idx = -1;
            for (int j=i+1; j<n; j++){
                if (tokens[j].end <= tokens[i+1].end) {
                    policy_object_end_idx = j;
                }
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx-1;
                    break;
                }
            }
            if (next_idx < 0) break;
        }
    }
    if (found_everyone != (0x1|0x2|0x4|0x8)) {
        proper_format = 0;
        goto proper_format_end;
    }

    // policy_object top level tokens
    if (tokens[policy_object_idx+1].type != JSMN_OBJECT ||
            tokens[policy_object_idx+1].size != 2) {
        proper_format = 0;
        goto proper_format_end;
    }
    int found_doc_goc = 0;
    for (int i = policy_object_idx; i < policy_object_end_idx; i++) {
        if (strncmp(&policy_data[tokens[i].start], "policy_doc", strlen("policy_doc")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {

            found_doc_goc |= 0x1;
            // jump to next same-level token
            int next_idx = -1;
            for (int j = i+1; j < policy_object_end_idx; j++) {
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx - 1;
                    break;
                }
            }
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_goc", strlen("policy_goc")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {
            found_doc_goc |= 0x2;

            // policy_goc contains keys "attribute_list" and "operation"
            // attribute_list value may be one of two types of arrays:
            //  - array of attributes
            //  - array of objects which contain attribute_list and operation
            for (int j = i+2; j < policy_object_end_idx; j++) {
                if (strncmp(&policy_data[tokens[j].start], "attribute_list", strlen("attribute_list")) == 0 &&
                        tokens[j].size == 1 &&
                        tokens[j+1].type == JSMN_ARRAY) {
                    found_doc_goc |= 0x4;
                    // jump to next same-level token
                    int next_idx = -1;
                    for (int k = j+1; k < policy_object_end_idx; k++) {
                        if (tokens[k].start >= tokens[j+1].end) {
                            next_idx = k;
                            j = next_idx - 1;
                            break;
                        }
                    }
                    if (next_idx < 0) break;
                } else if (strncmp(&policy_data[tokens[j].start], "operation", strlen("operation")) == 0 &&
                        tokens[j].size == 1 &&
                        tokens[j+1].type == JSMN_STRING) {
                    found_doc_goc |= 0x8;
                    // jump to next same-level token
                    int next_idx = -1;
                    for (int k = j+1; k < policy_object_end_idx; k++) {
                        if (tokens[k].start >= tokens[j+1].end) {
                            next_idx = k;
                            j = next_idx - 1;
                            break;
                        }
                    }
                    if (next_idx < 0) break;
                }
            }

            // jump to next same-level token
            int next_idx = -1;
            for (int j = i+1; j < policy_object_end_idx; j++) {
                if (tokens[j].start >= tokens[i+1].end) {
                    next_idx = j;
                    i = next_idx - 1;
                    break;
                }
            }
            if (next_idx < 0) break;
        }
    }
    if (found_doc_goc != (0x1|0x2|0x4|0x8)) {
        proper_format = 0;
        goto proper_format_end;
    }

    // check goc contents recursively

proper_format_end:
    report->proper_format = proper_format;
}
