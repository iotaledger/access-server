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
 * \file test_internal.c
 * \brief
 * Source file with implementation of test API for storage plugin
 *
 * @Author Strahinja Golic
 *
 * \notes
 * Plugins and PEP module must be initialized before test run
 *
 * \history
 * 27.05.2020. Initial version.
 ****************************************************************************/
/****************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "test_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apiorig.h"
#include "utils.h"

/****************************************************************************
 * MACROS
 ****************************************************************************/
#define TEST_ASCII_SPACE 32
#define TEST_BRACKETS_NUM 2
#define TEST_ASCII_TAB 9
#define TEST_ASCII_CR 13
#define TEST_ASCII_LF 10

#define TEST_CHECK_WHITESPACE(x) \
  ((x == TEST_ASCII_SPACE) || (x == TEST_ASCII_TAB) || (x == TEST_ASCII_CR) || (x == TEST_ASCII_LF) ? TRUE : FALSE)

/****************************************************************************
 * LOCAL VARIABLES
 ****************************************************************************/
static unsigned char public_key[TEST_PUBLIC_KEY_LEN];
static unsigned char private_key[TEST_PRIVATE_KEY_LEN];

/****************************************************************************
 * LOCAL FUNCTIONS
 ****************************************************************************/
static int normalize_object(char *object, int object_len, char **object_normalized) {
  char temp[object_len];
  int charCnt = 0;

  // Check input parameters
  if (object == NULL || object_len == 0) {
    printf("\nERROR[%s]: Bad input parameters.\n", __FUNCTION__);
    return 0;
  }

  // Normalize object and save it to temp
  memset(temp, 0, object_len);

  for (int i = 0; i < object_len; i++) {
    if (!TEST_CHECK_WHITESPACE(object[i])) {
      temp[charCnt] = object[i];
      charCnt++;
    }
  }

  // Allocate memory for object_normalized
  if (*object_normalized) {
    free(*object_normalized);
    *object_normalized = NULL;
  }

  *object_normalized = malloc(charCnt * sizeof(char));
  if (*object_normalized == NULL) {
    return 0;
  }

  // Copy temp to object_normalized
  memcpy(*object_normalized, temp, charCnt * sizeof(char));

  return charCnt;
}

/****************************************************************************
 * TEST API
 ****************************************************************************/
bool test_policy_storage() {
  bool ret = TRUE;
  char policy_id[TEST_POL_ID_MAX_LEN] = {0};
  char policy_id_hex[TEST_POL_ID_MAX_LEN / 2] = {0};
  char policy_id_hex_verified[TEST_POL_ID_MAX_LEN / 2] = {0};
  char signed_policy_id[TEST_POL_ID_MAX_LEN / 2 + TEST_SIGNATURE_LEN];
  char *policy_buffer = NULL;
  char *signed_policy_buffer = NULL;
  char *policy_object_buff = NULL;
  char *policy_object_pos = NULL;
  char *policy_object_norm = NULL;
  int buff_len = 0;
  int pol_object_norm_chars = 0;
  unsigned long long signed_buff_len = 0;
  unsigned long long signed_pol_id_ver_len = 0;
  FILE *f = NULL;
  pap_policy_t policy;

  // Generate keypair
  crypto_sign_keypair(public_key, private_key);

  // Load test policy to a buffer
  f = fopen("../../access/pap/validator/policies_test_set/policy_no1_ok.json", "r");
  if (f == NULL) {
    printf(
        "\nSTORAGE TEST FAILED - policy policy_no1_ok.json not available at acess/pap/validator/policies_test_set\n");
    return FALSE;
  }

  // Get file size
  fseek(f, 0L, SEEK_END);
  buff_len = ftell(f);
  fseek(f, 0L, SEEK_SET);

  // Store policy to buffer
  policy_buffer = malloc(buff_len * sizeof(char));
  policy_object_buff = malloc(buff_len * sizeof(char));  // Worst case
  policy_object_norm = malloc(buff_len * sizeof(char));  // Worst case
  signed_policy_buffer = malloc((buff_len + TEST_SIGNATURE_LEN) * sizeof(char));

  fread(policy_buffer, buff_len * sizeof(char), 1, f);
  fclose(f);

  // Sign policy
  crypto_sign(signed_policy_buffer, &signed_buff_len, policy_buffer, buff_len, private_key);

  // Add signed policy
  if (ret && (pap_add_policy(signed_policy_buffer, signed_buff_len, policy_id, public_key) == PAP_ERROR)) {
    printf("\nSTORAGE TEST FAILED - Couldn't add policy\n");
    ret = FALSE;
  }

  // Check if policy is added
  if (ret && (pap_has_policy(policy_id, TEST_POL_ID_MAX_LEN) == FALSE)) {
    printf("\nSTORAGE TEST FAILED - Policy is not added\n");
    ret = FALSE;
  }

  policy.policy_object.policy_object = policy_object_buff;
  // Recover policy
  if (ret && (pap_get_policy(policy_id, TEST_POL_ID_MAX_LEN, &policy) == PAP_ERROR)) {
    printf("\nSTORAGE TEST FAILED - Couldn't add policy\n");
    ret = FALSE;
  }

  // Check recovered values
  if (ret && str_to_hex(policy_id, policy_id_hex, TEST_POL_ID_MAX_LEN) != UTILS_STRING_SUCCESS) {
    printf("\nSTORAGE TEST FAILED - Couldn't convert string to hex\n");
    ret = FALSE;
  }

  if (ret && (memcmp(policy_id_hex, policy.policy_id, TEST_POL_ID_MAX_LEN / 2) != 0)) {
    printf("\nSTORAGE TEST FAILED - Policy ID hex differ from recovered value\n");
    ret = FALSE;
  } else {
    policy_object_pos = strstr(policy_buffer, "policy_object");
    pol_object_norm_chars =
        normalize_object(policy_object_pos, &policy_buffer[buff_len] - policy_object_pos, &policy_object_norm);
  }

  if (ret && ((pol_object_norm_chars - strlen("policy_object:") - TEST_BRACKETS_NUM) !=
              policy.policy_object.policy_object_size)) {
    printf("\nSTORAGE TEST FAILED - Policy object size differ from recovered value\n");
    ret = FALSE;
  }

  if (ret && (memcmp(&policy_object_norm[strlen("policy_object:{")], policy.policy_object.policy_object,
                     policy.policy_object.policy_object_size) != 0)) {
    printf("\nSTORAGE TEST FAILED - Policy object differ from recovered value\n");
    ret = FALSE;
  }

  if (ret && (policy.policy_id_signature.signature_algorithm != PAP_ECDSA)) {
    printf("\nSTORAGE TEST FAILED - Policy ID signature algorithm differ from recovered value\n");
    ret = FALSE;
  } else {
    // Prepend signature to policy_id_hex
    memcpy(signed_policy_id, policy.policy_id_signature.signature, TEST_SIGNATURE_LEN);
    memcpy(&signed_policy_id[TEST_SIGNATURE_LEN], policy_id_hex, TEST_POL_ID_MAX_LEN / 2);
  }

  if (ret &&
      (crypto_sign_open(policy_id_hex_verified, &signed_pol_id_ver_len, signed_policy_id,
                        TEST_SIGNATURE_LEN + TEST_POL_ID_MAX_LEN / 2, policy.policy_id_signature.public_key) != 0)) {
    printf("\nSTORAGE TEST FAILED - Policy ID signature differ from recovered value\n");
    ret = FALSE;
  }

  if (ret && (policy.hash_function != PAP_SHA_256)) {
    printf("\nSTORAGE TEST FAILED - Hash function differ from recovered value\n");
    ret = FALSE;
  }

  // Delete policy
  if (ret && (pap_remove_policy(policy_id, TEST_POL_ID_MAX_LEN) == PAP_ERROR)) {
    printf("\nSTORAGE TEST FAILED - Policy is not removed\n");
    ret = FALSE;
  }

  if (ret) {
    printf("\nSTORAGE TEST PASSED!!!\n");
  }

  free(policy_buffer);
  free(policy_object_buff);
  free(policy_object_norm);
  free(signed_policy_buffer);
  return ret;
}
