#ifndef L5_FS_API_H
#define L5_FS_API_H

#define CRYPTO_SECRETKEYBYTES 64
#define CRYPTO_PUBLICKEYBYTES 32
#define CRYPTO_BYTES (4 + 132824)
#define CRYPTO_ALGNAME "picnicl5fs"
#define CRYPTO_VERSION "1.1"

int crypto_sign_keypair(unsigned char* pk, unsigned char* sk);
int crypto_sign(unsigned char* sm, unsigned long long* smlen, const unsigned char* m,
                unsigned long long mlen, const unsigned char* sk);
//@FIXME: There seems to be a bug in this function. Should be evaluated before usage.
int crypto_sign_detached(unsigned char *sig, unsigned long long *siglen_p,
                              const unsigned char *m, unsigned long long mlen,
                              const unsigned char *sk);
int crypto_sign_open(unsigned char* m, unsigned long long* mlen, const unsigned char* sm,
                     unsigned long long smlen, const unsigned char* pk);
//@FIXME: There seems to be a bug in this function. Should be evaluated before usage.
int crypto_sign_verify_detached(const unsigned char *sig,
                                const unsigned char *m,
                                unsigned long long mlen,
                                const unsigned char *pk);

#endif
