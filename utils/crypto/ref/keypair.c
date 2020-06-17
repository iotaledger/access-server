#include <stdlib.h>
#include <string.h>
#include "crypto_hash_sha512.h"
#include "ge25519.h"

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
  unsigned char az[64];
  sc25519 scsk;
  ge25519 gepk;

  unsigned char tmp[32];  //= { 0xaa, 0xaa, 0xbb, 0x86, 0xfa, 0xd1, 0x9a, 0xa5, 0xd9,
                          // 0xb2, 0xdb, 0x38, 0x43, 0x92, 0x54, 0x37, 0x72, 0x3e, 0xd3,
                          // 0x4b, 0xd1, 0x93, 0x8f, 0x2d, 0x9d, 0x8b, 0x3d, 0xb5, 0x2a,
                          // 0x9d, 0xf9, 0x60};

  for (int i = 0; i < 32; i++) {
    tmp[i] = (unsigned char)(rand() % 256);
  }
  memcpy(sk, tmp, 32);
  // randombytes(sk,32);
  crypto_hash_sha512(az, sk, 32);
  az[0] &= 248;
  az[31] &= 127;
  az[31] |= 64;

  sc25519_from32bytes(&scsk, az);

  ge25519_scalarmult_base(&gepk, &scsk);
  ge25519_pack(pk, &gepk);
  memmove(sk + 32, pk, 32);
  return 0;
}
