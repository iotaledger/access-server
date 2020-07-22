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
 * File:        validate.c
 *
 * Designed-by: Djordje Golubovic
 *
 * History:     21.02.2020. - Initial version
 *****************************************************************************/

/***************************************************************************
 * INCLUDES
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "optimizer.h"
#include "validator.h"

/***************************************************************************
 * DEFINES
 ****************************************************************************/
#define MAX_POLICY_FILE_SIZE 4096
#define FREAD_CHUNK 512
#define YESNO(condition) (condition) ? "yes" : "no"

/***************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static char policy_data[MAX_POLICY_FILE_SIZE] = "";
static int policy_data_len = 0;

/***************************************************************************
 * FUNCTION IMPLEMENTATIONS
 ****************************************************************************/

/*
 *  Function: main
 *  Description: Main Function
 *  Parameters: argc - num of parameters
 *              argv - path to .json file
 *  Returns: 0 - success,
 *           1 - fail
 */
int main(int argc, char** argv) {
  FILE* fp;

  // Check arguments
  if (argc < 2) {
    printf("Input file required!\n");
    return 1;
  }

  // Open .json file
  fp = fopen(argv[1], "r");

  if (fp == NULL) {
    printf("No such file: %s\n", argv[1]);
    exit(1);
  }

  // Read file
  while (!feof(fp)) {
    int read_bytes = fread(&policy_data[policy_data_len], 1, FREAD_CHUNK, fp);
    policy_data_len += read_bytes;
    if (policy_data_len >= 4096) {
      break;
    }
  }

  // Validate
  fclose(fp);
  validator_report_t report;
  validator_check(policy_data, &report);

  printf("validator report:\n - valid json: %s\n - proper format: %s\n", YESNO(report.valid_json == 1),
         YESNO(report.proper_format == 1));

  if (argc >= 3) {
    optimizer_optimize_pol(policy_data, argv[2]);
  }

  return 0;
}
