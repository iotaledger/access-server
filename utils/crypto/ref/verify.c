/*
 * verify.c
 *
 *  Created on: Nov 28, 2018
 *      Author: milivoje.knezevic
 */

#include <stddef.h>
#include <stdint.h>

#include "crypto_verify_32.h"

static inline int crypto_verify_n(const unsigned char *x_, const unsigned char *y_, const int n) {
  const volatile unsigned char *volatile x = (const volatile unsigned char *volatile)x_;
  const volatile unsigned char *volatile y = (const volatile unsigned char *volatile)y_;
  volatile uint_fast16_t d = 0U;
  int i;

  for (i = 0; i < n; i++) {
    d |= x[i] ^ y[i];
  }
  return (1 & ((d - 1) >> 8)) - 1;
}

int crypto_verify_32(const unsigned char *x, const unsigned char *y) {
  return crypto_verify_n(x, y, crypto_verify_32_BYTES);
}
