#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "validator.h"

#define MAX_POLICY_FILE_SIZE 4096

static char policy_data[MAX_POLICY_FILE_SIZE] = "";
static int policy_data_len = 0;

int main(int argc, char** argv) {
    FILE *fp;

    fp = fopen("policy.json", "r");

    if (fp == NULL) {
        printf("No such file: policy.json\n");
        exit(1);
    }

    while (!feof(fp)) {
        int read_bytes = fread(&policy_data[policy_data_len], 512, 1, fp);
        policy_data_len += read_bytes;
        if (policy_data_len >= 4096) break;
    }

    fclose(fp);
    Validator_report_t report;
    Validator_check(policy_data, &report);

    return 0;
}
