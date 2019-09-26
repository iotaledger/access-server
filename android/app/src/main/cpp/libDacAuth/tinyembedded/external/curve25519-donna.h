#ifndef _CURVE25519_DONNA
#define _CURVE25519_DONNA

typedef unsigned char u8;
int curve25519_donna(u8 *mypublic, const u8 *secret, const u8 *basepoint);

#endif
