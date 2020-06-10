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

#include <inttypes.h>

#include "unity/unity.h"

#include "wallet.h"

#define TEST_NODE_ENDPOINT "nodes.iota.cafe"
#define TEST_NODE_PORT 443
#define TEST_NODE_MWM 3
#define TEST_NODE_DEPTH 14

wallet_ctx_t *wallet;

static char const *amazon_ca1_pem =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n"
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n"
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n"
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n"
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n"
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n"
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n"
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n"
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n"
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n"
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n"
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n"
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n"
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n"
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n"
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n"
    "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"
    "-----END CERTIFICATE-----\r\n";

static char const *test_seed = "ABCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
static char const *empty_address = "999999999999999999999999999999999999999999999999999999999999999999999999999999999";

void test_create_invalid_seed(void) {
  wallet_ctx_t *w = wallet_create(TEST_NODE_ENDPOINT, TEST_NODE_PORT, amazon_ca1_pem, TEST_NODE_MWM, TEST_NODE_DEPTH,
                                  "ABCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  TEST_ASSERT_NULL(w);
  wallet_destory(&w);
  TEST_ASSERT_NULL(w);
}

void test_create_destory_wallet(void) {
  wallet_ctx_t *w =
      wallet_create(TEST_NODE_ENDPOINT, TEST_NODE_PORT, amazon_ca1_pem, TEST_NODE_MWM, TEST_NODE_DEPTH, test_seed);
  TEST_ASSERT_NOT_NULL(w);
  TEST_ASSERT_NOT_NULL(w->iota_client);
  TEST_ASSERT_EQUAL_UINT16(w->iota_client->http.port, 443);
  TEST_ASSERT_EQUAL_UINT8(w->depth, 3);
  TEST_ASSERT_EQUAL_UINT32(w->mwm, 14);
  TEST_ASSERT_EQUAL_MEMORY(w->seed, test_seed, NUM_TRYTES_HASH);
  wallet_destory(&w);
  TEST_ASSERT_NULL(w);
}

void test_wallet_check_balance_invalid_addr(void) {
  uint64_t balance = 0;
  wallet_err_t ret = wallet_check_balance(wallet, "R9TLTDMFUNY", &balance);

  TEST_ASSERT(ret != WALLET_OK);
}

void test_wallet_check_balance(void) {
  uint64_t balance = 0;
  wallet_err_t ret = wallet_check_balance(
      wallet, "UCDZKUEEGVBHBTRTYHQSWVFWURTOPSMUCOFTNXKAHATMTKLJIDQLGGULNOZZGELOGVYFIBR9TLTDMFUNY", &balance);

  TEST_ASSERT(ret == WALLET_OK);
}

void test_wallet_get_address(void) {
  char unused_addr[NUM_TRYTES_ADDRESS] = {0};
  uint64_t unused_index = 0;
  memcpy(unused_addr, empty_address, NUM_TRYTES_HASH);
  TEST_ASSERT(memcmp(unused_addr, empty_address, NUM_TRYTES_HASH) == 0);

  wallet_err_t ret = wallet_get_address(wallet, unused_addr, &unused_index);
  TEST_ASSERT(ret == WALLET_OK);
  // printf("unused addr:%s, index: %ld\n", unused_addr, unused_index);

  TEST_ASSERT(memcmp(unused_addr, empty_address, NUM_TRYTES_HASH) != 0);
}

void test_wallet_send(void) {
  char const *test_receiver = "UCDZKUEEGVBHBTRTYHQSWVFWURTOPSMUCOFTNXKAHATMTKLJIDQLGGULNOZZGELOGVYFIBR9TLTDMFUNY";
  char const *test_msg =
      "{ \"id\": \"DLKJELKJDLE\", \"name\": \"Foo\", \"price\": 123, \"tags\": [ \"Bar\", \"Eek\" ], \"stock\": { "
      "\"warehouse\": 300, \"retail\": 20 } }";
  char bundle_hash[NUM_TRYTES_BUNDLE + 1] = {};

  wallet_err_t ret = wallet_send(wallet, test_receiver, 0, test_msg, bundle_hash);
  TEST_ASSERT(ret == WALLET_OK);
  TEST_ASSERT(memcmp(bundle_hash, empty_address, NUM_TRYTES_HASH) != 0);

  bundle_hash[NUM_TRYTES_BUNDLE] = '\0';
  printf("bundle: %s\n", bundle_hash);
}

static void balance_notifier(uint64_t start, uint64_t end) {
  printf("balance service callback: start %" PRIu64 ", end %" PRIu64 "\n", start, end);
}

void test_balance_service(void) {
  balance_service_t *service =
      balance_service_start(wallet, "UCDZKUEEGVBHBTRTYHQSWVFWURTOPSMUCOFTNXKAHATMTKLJIDQLGGULNOZZGELOGVYFIBR9TLTDMFUNY",
                            30, 10, balance_notifier);

  TEST_ASSERT_NOT_NULL(service);
  sleep(60);
  balance_service_stop(service);
  pthread_join(service->thread_id, NULL);
  balance_service_free(&service);
  TEST_ASSERT_NULL(service);
}

void test_wallet_check_confirmation(void) {
  char const *tx_hash1 = "99ASUHT9RQXQJYXAQQZXYRLQIBBBAAKUKMIGPABOFORRPCYIIFWLNECDLRHWGBFMNTKTIARTPPEI99999";
  bool confirmed = wallet_check_confirmation(wallet, tx_hash1);
  printf("%s : %s\n", tx_hash1, confirmed ? "true" : "false");

  char const *tx_hash2 = "ITEIBIZOSGDINVWJEAJALFLRQNSNX9HWCNXLNQMISCGHVOQZTV9BFIYNZNYFHKRACGIOMISTGCTQ99999";
  confirmed = wallet_check_confirmation(wallet, tx_hash2);
  printf("%s : %s\n", tx_hash2, confirmed ? "true" : "false");
}

static void confirmation_notifier(uint32_t time, bool is_confirmed, pthread_t thread_id) { printf("confirmation callback in %d \n", time); }

void test_confirmation_service(void) {
  confirmation_service_t *service = confirmation_service_start(
      wallet, "99ASUHT9RQXQJYXAQQZXYRLQIBBBAAKUKMIGPABOFORRPCYIIFWLNECDLRHWGBFMNTKTIARTPPEI99999", 5, 30,
      confirmation_notifier);

  TEST_ASSERT_NOT_NULL(service);
  sleep(40);
  // balance_service_stop(service);
  pthread_join(service->thread_id, NULL);
  confirmation_service_free(&service);
  TEST_ASSERT_NULL(service);
}

int main() {
  UNITY_BEGIN();
  wallet = wallet_create(TEST_NODE_ENDPOINT, TEST_NODE_PORT, amazon_ca1_pem, TEST_NODE_MWM, TEST_NODE_DEPTH, test_seed);
  TEST_ASSERT_NOT_NULL(wallet);

  RUN_TEST(test_create_invalid_seed);
  RUN_TEST(test_create_destory_wallet);
  RUN_TEST(test_wallet_check_balance);
  RUN_TEST(test_wallet_check_balance_invalid_addr);
  RUN_TEST(test_wallet_get_address);
  // RUN_TEST(test_wallet_send);
  // RUN_TEST(test_balance_service);
  RUN_TEST(test_wallet_check_confirmation);
  // RUN_TEST(test_confirmation_service);

  wallet_destory(&wallet);
  TEST_ASSERT_NULL(wallet);
  return UNITY_END();
}