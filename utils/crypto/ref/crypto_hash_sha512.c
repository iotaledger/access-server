/*
 * crypto_hash_sha512.c
 *
 *  Created on: Nov 28, 2018
 *      Author: milivoje.knezevic
 */

#include "crypto_hash_sha512.h"

int crypto_hash_sha512(unsigned char *out, const unsigned char *in, unsigned long long inlen) {
  sha512_context ctx;
  sha512_init(&ctx);
  sha512_update(&ctx, in, inlen);
  sha512_final(&ctx, out);

  return 0;
}
