#include <string.h>
#include "crypto_sign.h"
#include "crypto_hash_sha512.h"
#include "ge25519.h"
#include "utils.h"

void crypto_sign_ed25519_ref10_hinit(crypto_hash_sha512_state *hs, int prehashed)
{
    static const unsigned char DOM2PREFIX[32 + 2] = {
        'S', 'i', 'g', 'E', 'd', '2', '5', '5', '1', '9', ' ',
        'n', 'o', ' ',
        'E', 'd', '2', '5', '5', '1', '9', ' ',
        'c', 'o', 'l', 'l', 'i', 's', 'i', 'o', 'n', 's', 1, 0
    };

    crypto_hash_sha512_init(hs);
    if (prehashed) {
        crypto_hash_sha512_update(hs, DOM2PREFIX, sizeof DOM2PREFIX);
    }
}

void crypto_sign_ed25519_clamp(unsigned char k[32])
{
    k[0] &= 248;
    k[31] &= 127;
    k[31] |= 64;
}

int crypto_sign(
    unsigned char *sm,unsigned long long *smlen,
    const unsigned char *m,unsigned long long mlen,
    const unsigned char *sk
    )
{
  unsigned char pk[32];
  unsigned char az[64];
  unsigned char nonce[64];
  unsigned char hram[64];
  sc25519 sck, scs, scsk;
  ge25519 ger;

  memmove(pk,sk + 32,32);
  /* pk: 32-byte public key A */

  crypto_hash_sha512(az,sk,32);
  az[0] &= 248;
  az[31] &= 127;
  az[31] |= 64;
  /* az: 32-byte scalar a, 32-byte randomizer z */

  *smlen = mlen + 64;
  memmove(sm + 64,m,mlen);
  memmove(sm + 32,az + 32,32);
  /* sm: 32-byte uninit, 32-byte z, mlen-byte m */

  crypto_hash_sha512(nonce, sm+32, mlen+32);
  /* nonce: 64-byte H(z,m) */

  sc25519_from64bytes(&sck, nonce);
  ge25519_scalarmult_base(&ger, &sck);
  ge25519_pack(sm, &ger);
  /* sm: 32-byte R, 32-byte z, mlen-byte m */
  
  memmove(sm + 32,pk,32);
  /* sm: 32-byte R, 32-byte A, mlen-byte m */

  crypto_hash_sha512(hram,sm,mlen + 64);
  /* hram: 64-byte H(R,A,m) */

  sc25519_from64bytes(&scs, hram);
  sc25519_from32bytes(&scsk, az);
  sc25519_mul(&scs, &scs, &scsk);
  sc25519_add(&scs, &scs, &sck);
  /* scs: S = nonce + H(R,A,m)a */

  sc25519_to32bytes(sm + 32,&scs);
  /* sm: 32-byte R, 32-byte S, mlen-byte m */

  return 0;
}

int crypto_sign_detached(unsigned char *sig, unsigned long long *siglen_p,
                              const unsigned char *m, unsigned long long mlen,
                              const unsigned char *sk)
{
    crypto_hash_sha512_state hs;
    unsigned char            az[64];
    unsigned char            nonce_c[64];
    sc25519                  nonce;
    unsigned char            hram[64];
    ge25519_p3               R;

    crypto_sign_ed25519_ref10_hinit(&hs, 0);

    crypto_hash_sha512(az, sk, 32);

    crypto_hash_sha512_update(&hs, az + 32, 32);

    crypto_hash_sha512_update(&hs, m, mlen);
    crypto_hash_sha512_final(&hs, nonce_c);

    memmove(sig + 32, sk + 32, 32);

    sc25519_reduce(nonce_c);
    for(int i = 0; i < 32; i++){
        nonce.v[i] = nonce_c[i];
    }
    ge25519_scalarmult_base(&R, &nonce);
    ge25519_p3_tobytes(sig, &R);

    crypto_sign_ed25519_ref10_hinit(&hs, 0);
    crypto_hash_sha512_update(&hs, sig, 64);
    crypto_hash_sha512_update(&hs, m, mlen);
    crypto_hash_sha512_final(&hs, hram);

    sc25519_reduce(hram);
    crypto_sign_ed25519_clamp(az);
    for(int i = 0; i < 32; i++){
        nonce_c[i] = nonce.v[i];
    }
    sc25519_muladd(sig + 32, hram, az, nonce_c);

    sodium_memzero(az, sizeof az);
    sodium_memzero(nonce_c, sizeof nonce_c);

    if (siglen_p != NULL) {
        *siglen_p = 64U;
    }
    return 0;
}
