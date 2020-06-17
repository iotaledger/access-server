/****************************************************************************
 *  Copyright (c) 2018 XAIN
 *
 *  All Rights Reserved
 *
 *  P R O P R I E T A R Y    &    C O N F I D E N T I A L
 *
 *  -----------------------------------------------------
 *  https://xain.io/
 *  -----------------------------------------------------
 *
 * \project Decentralized Access Control
 * \file asn_debug.h
 * \brief
 * Definition of macros used for debugging.
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 14.08.2018. Initial version.
 ****************************************************************************/

#ifndef ASN_DEBUG_H_
#define ASN_DEBUG_H_

#include "dlog.h"

#define RESET "\033[0m"
#define RED "\033[31m"
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...)                    \
  dlog_printf(stderr,                    \
              "[" BOLDBLUE "DEBUG" RESET \
              "]"                        \
              "%s:%d:" M "\n",           \
              __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...)                 \
  fprintf(stderr,                       \
          "[" BOLDRED "ERROR" RESET     \
          "]"                           \
          "(%s:%d: errno: %s) " M "\n", \
          __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...)               \
  fprintf(stderr,                      \
          "[" BOLDMAGENTA "WARN" RESET \
          "]"                          \
          "(%s:%d: errno:%s) " M "\n", \
          __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...)             \
  fprintf(stderr,                    \
          "[" BOLDBLACK "INFO" RESET \
          "]"                        \
          "(%s:%d) " M "\n",         \
          __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...)       \
  if (!(A)) {                  \
    log_err(M, ##__VA_ARGS__); \
    errno = 0;                 \
    goto error;                \
  }

#define sentinel(M, ...)       \
  {                            \
    log_err(M, ##__VA_ARGS__); \
    errno = 0;                 \
    goto error;                \
  }

#define check_mem(A) check((A), "Out of memory.")

#define check_debug(A, M, ...) \
  if (!(A)) {                  \
    debug(M, ##__VA_ARGS__);   \
    errno = 0;                 \
    goto error;                \
  }

#endif /* ASN_DEBUG_H_ */
