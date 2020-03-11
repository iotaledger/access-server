#include "validator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSMN_HEADER
#include "jsmn.h"

char token_str[64];

static int next_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx) {
    int next_idx = -1;
    // only tokens of size 1 can have siblings
    if (_tokens[cur_idx].size == 1) {
        for (int i = cur_idx+1; i < max_idx; i++) {
            if (_tokens[i].start >= _tokens[cur_idx+1].end) {
                next_idx = i;
                break;
            }
        }
    }
    return next_idx;
}

static int end_of_current_idx(jsmntok_t* _tokens, int cur_idx, int max_idx) {
    int end_of_current = -1;
    for (int i=cur_idx+1; i<max_idx; i++){
        if (_tokens[i].end <= _tokens[cur_idx+1].end) {
            end_of_current = i;
        }
        if (_tokens[i].start >= _tokens[cur_idx+1].end) {
            break;
        }
    }
    return end_of_current;
}



static void iterate_json_array(jsmntok_t* _tokens, int array_idx, void (*iter_func)(jsmntok_t* _toks, int el_idx)) {
    if (iter_func != NULL &&
            _tokens[array_idx].type == JSMN_ARRAY &&
            _tokens[array_idx].size > 0 &&
            _tokens[array_idx+1].type = JSMN_OBJECT &&
            _tokens[array_idx+1].size > 0) {
        int cur_idx = array_idx + 1;
        for (int i = 0; i < _tokens[array_idx].size; i++) {
            iter_func(_tokens, cur_idx);
            cur_idx = end_of_current_idx(_tokens, cur_idx, _tokens[array_idx].size + cur_idx);
        }
    }
}

static int check_goc(jsmntok_t* _tokens, int goc_idx, int max_idx, const char* policy_data) {
    for (int i = goc_idx+2, i < max_idx; i++) {
        if (strncmp(&policy_data[tokens[i].start], "attribute_list", strlen("attribute_list")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_ARRAY &&
                tokens[i+1].size > 0) {
            int tmp_idx = 0;
            for (int j = 0; j < tokens[i+1].size; j++) {
                if (strncmp("attribute_list",
                        policy_data[tokens[i+2+tmp_idx].start],
                        strlen("attribute_list")) == 0 ||
                    strncmp("operation",
                        policy_data[tokens[i+2+tmp_idx].start],
                        strlen("operation")) == 0) {

                }
            }
            int next_idx = next_sibling_idx(_tokens, i, max_idx);
        } else if (strncmp(&policy_data[tokens[i].start], "operation", strlen("operation")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_ARRAY) {
            int next_idx = next_sibling_idx(_tokens, i, max_idx);
        }
    }
    return 0;
}

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

            int next_idx = next_sibling_idx(tokens, i, n);
            i = next_idx - 1;
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "hash_function", strlen("hash_function")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].size == 0 &&
                tokens[i+1].type == JSMN_STRING) {
            // @TODO check if value token is in valid range, type, etc
            found_everyone |= 0x02;

            int next_idx = next_sibling_idx(tokens, i, n);
            i = next_idx - 1;
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_id", strlen("policy_id")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].size == 0 &&
                tokens[i+1].type == JSMN_STRING) {
            // @TODO check if value token is in valid range, type, etc
            found_everyone |= 0x04;

            int next_idx = next_sibling_idx(tokens, i, n);
            i = next_idx - 1;
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_object", strlen("policy_object")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {
            found_everyone |= 0x08;
            policy_object_idx = i;

            int next_idx = -1;
            policy_object_end_idx = end_of_current_idx(tokens, i, n);
            next_idx = next_sibling_idx(tokens, i, n);
            i = next_idx - 1;
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
    int policy_goc_idx = -1;
    for (int i = policy_object_idx; i < policy_object_end_idx; i++) {
        if (strncmp(&policy_data[tokens[i].start], "policy_doc", strlen("policy_doc")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {

            found_doc_goc |= 0x1;
            int next_idx = next_sibling_idx(tokens, i, policy_object_end_idx);
            if (next_idx < 0) break;
        } else if (strncmp(&policy_data[tokens[i].start], "policy_goc", strlen("policy_goc")) == 0 &&
                tokens[i].size == 1 &&
                tokens[i+1].type == JSMN_OBJECT) {
            found_doc_goc |= 0x2;
            policy_goc_idx = i;
            // policy_goc contains keys "attribute_list" and "operation"
            // attribute_list value may be one of two types of arrays:
            //  - array of attributes
            //  - array of objects which contain attribute_list and operation
            for (int j = i+2; j < policy_object_end_idx; j++) {
                if (strncmp(&policy_data[tokens[j].start], "attribute_list", strlen("attribute_list")) == 0 &&
                        tokens[j].size == 1 &&
                        tokens[j+1].type == JSMN_ARRAY) {
                    found_doc_goc |= 0x4;

                    int next_idx = next_sibling_idx(tokens, j, policy_object_end_idx);
                    if (next_idx < 0) break;
                } else if (strncmp(&policy_data[tokens[j].start], "operation", strlen("operation")) == 0 &&
                        tokens[j].size == 1 &&
                        tokens[j+1].type == JSMN_STRING) {
                    found_doc_goc |= 0x8;
                    // jump to next same-level token
                    int next_idx = next_sibling_idx(tokens, j, policy_object_end_idx);
                    if (next_idx < 0) break;
                }
            }

            // jump to next same-level token
            int next_idx = next_sibling_idx(tokens, i, policy_object_end_idx);
            if (next_idx < 0) break;
        }
    }
    if (found_doc_goc != (0x1|0x2|0x4|0x8)) {
        proper_format = 0;
        goto proper_format_end;
    }

    // check goc contents recursively
    int ttt = check_goc(tokens, policy_goc_idx);


proper_format_end:
    report->proper_format = proper_format;
}
