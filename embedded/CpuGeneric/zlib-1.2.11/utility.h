#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"
#include "policystore.h"
#include <stdlib.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 1024

int def(FILE *source, FILE *dest, int level);

int def_mem(char *source, policy_t *pol, int level);

int inf_mem_to_file(policy_t *pol, FILE *dest);

int inf_mem(policy_t *pol, unsigned char *dest);

int inf(FILE *source, FILE *dest);

void zerr(int ret);
