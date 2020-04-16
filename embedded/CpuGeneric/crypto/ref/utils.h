#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

uint64_t load_3(const unsigned char *in);
uint64_t load_4(const unsigned char *in);
int sodium_is_zero(const unsigned char *n, const size_t nlen);
void sodium_memzero(void * const pnt, const size_t len);
int sodium_memcmp(const void * const b1_, const void * const b2_, size_t len);

#endif
