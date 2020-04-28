#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "validator.h"

#define MAX_POLICY_FILE_SIZE 4096

static char policy_data[MAX_POLICY_FILE_SIZE] = "";
static int policy_data_len = 0;

int main(int argc, char** argv) {
    FILE *fp;

    if (argc != 2) {
        printf("Input file required!\n");
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("No such file: %s\n", argv[1]);
        exit(1);
    }

    while (!feof(fp)) {
        int read_bytes = fread(&policy_data[policy_data_len], 1, 512, fp);
        policy_data_len += read_bytes;
        if (policy_data_len >= 4096) break;
    }

    fclose(fp);
    Validator_report_t report;
    Validator_check(policy_data, &report);

#define YESNO(condition) (condition) ? "yes" : "no"

    printf("validator report:\n - valid json: %s\n - proper format: %s\n",
        YESNO(report.valid_json == 1),
        YESNO(report.proper_format == 1));

    return 0;
}
