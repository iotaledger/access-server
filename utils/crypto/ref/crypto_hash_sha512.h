#ifndef CRYPTO_HASH_SHA_H_
#define CRYPTO_HASH_SHA_H_

#include "sha512.h"

int crypto_hash_sha512(unsigned char *out, const unsigned char *in, unsigned long long inlen);
#endif
