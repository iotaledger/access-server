#ifndef SC25519_H
#define SC25519_H

#include "crypto_int32.h"
#include "crypto_uint32.h"

#define sc25519 crypto_sign_ed25519_ref_sc25519
#define sc25519_from32bytes crypto_sign_ed25519_ref_sc25519_from32bytes
#define sc25519_from64bytes crypto_sign_ed25519_ref_sc25519_from64bytes
#define sc25519_to32bytes crypto_sign_ed25519_ref_sc25519_to32bytes
#define sc25519_add crypto_sign_ed25519_ref_sc25519_add
#define sc25519_mul crypto_sign_ed25519_ref_sc25519_mul
#define sc25519_window3 crypto_sign_ed25519_ref_sc25519_window3
#define sc25519_2interleave2 crypto_sign_ed25519_ref_sc25519_2interleave2

typedef struct {
  crypto_uint32 v[32];
} sc25519;

void sc25519_from32bytes(sc25519 *r, const unsigned char x[32]);

void sc25519_from64bytes(sc25519 *r, const unsigned char x[64]);

void sc25519_to32bytes(unsigned char r[32], const sc25519 *x);

void sc25519_add(sc25519 *r, const sc25519 *x, const sc25519 *y);

void sc25519_mul(sc25519 *r, const sc25519 *x, const sc25519 *y);

/* Convert s into a representation of the form \sum_{i=0}^{84}r[i]2^3
 * with r[i] in {-4,...,3}
 */
void sc25519_window3(signed char r[85], const sc25519 *s);

void sc25519_2interleave2(unsigned char r[127], const sc25519 *s1, const sc25519 *s2);

#endif
