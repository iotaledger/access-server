#ifndef CRYPTO_HASH_SHA_H_
#define CRYPTO_HASH_SHA_H_

#include "sha512.h"

typedef struct crypto_hash_sha512_state {
    uint64_t state[8];
    uint64_t count[2];
    uint8_t  buf[128];
} crypto_hash_sha512_state;

int crypto_hash_sha512(unsigned char *out, const unsigned char *in,
                   unsigned long long inlen);

int crypto_hash_sha512_init(crypto_hash_sha512_state *state);

int crypto_hash_sha512_update(crypto_hash_sha512_state *state,
                              const unsigned char *in,
                              unsigned long long inlen);

int crypto_hash_sha512_final(crypto_hash_sha512_state *state,
                             unsigned char *out);
#endif
