#ifndef GE25519_H
#define GE25519_H

#include "fe25519.h"
#include "sc25519.h"

#ifndef CRYPTO_ALIGN
# if defined(__INTEL_COMPILER) || defined(_MSC_VER)
#  define CRYPTO_ALIGN(x) __declspec(align(x))
# else
#  define CRYPTO_ALIGN(x) __attribute__ ((aligned(x)))
# endif
#endif

#define COMPILER_ASSERT(X) (void) sizeof(char[(X) ? 1 : -1])


#define ge25519                           crypto_sign_ed25519_ref_ge25519
#define ge25519_base                      crypto_sign_ed25519_ref_ge25519_base
#define ge25519_unpackneg_vartime         crypto_sign_ed25519_ref_unpackneg_vartime
#define ge25519_pack                      crypto_sign_ed25519_ref_pack
#define ge25519_isneutral_vartime         crypto_sign_ed25519_ref_isneutral_vartime
#define ge25519_double_scalarmult_vartime crypto_sign_ed25519_ref_double_scalarmult_vartime
#define ge25519_scalarmult_base           crypto_sign_ed25519_ref_scalarmult_base

#define ge25519_p3 ge25519

typedef struct
{
  fe25519 x;
  fe25519 y;
  fe25519 z;
  fe25519 t;
} ge25519;

typedef struct
{
  fe25519 x;
  fe25519 z;
  fe25519 y;
  fe25519 t;
} ge25519_p1p1;

typedef struct
{
  fe25519 x;
  fe25519 y;
  fe25519 z;
} ge25519_p2;

typedef struct {
  fe25519 yplusx;
  fe25519 yminusx;
  fe25519 xy2d;
} ge25519_precomp;

typedef struct {
  fe25519 YplusX;
  fe25519 YminusX;
  fe25519 Z;
  fe25519 T2d;
} ge25519_cached;

void ge25519_add(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q);

void ge25519_sub(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q);

int ge25519_unpackneg_vartime(ge25519 *r, const unsigned char p[32]);

void ge25519_pack(unsigned char r[32], const ge25519 *p);

int ge25519_isneutral_vartime(const ge25519 *p);

void ge25519_double_scalarmult_vartime(ge25519_p3 *r, const ge25519_p3 *p1, const sc25519 *s1, const ge25519_p3 *p2, const sc25519 *s2);

void ge25519_double_scalarmult_vartime_new(ge25519_p2 *r, const unsigned char *a, const ge25519_p3 *A, const unsigned char *b);

void ge25519_scalarmult_base(ge25519 *r, const sc25519 *s);

int ge25519_has_small_order(const unsigned char s[32]);

int ge25519_is_canonical(const unsigned char *s);

int ge25519_frombytes_negate_vartime(ge25519_p3 *h, const unsigned char *s);

void ge25519_tobytes(unsigned char *s, const ge25519_p2 *h);

#endif
