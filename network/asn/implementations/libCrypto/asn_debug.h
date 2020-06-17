/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/****************************************************************************
 * \project IOTA Access
 * \file asn_debug.h
 * \brief
 * Debug macros for this module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 05.05.2020. Initial version.
 ****************************************************************************/

#ifndef ASN_DEBUG_H_
#define ASN_DEBUG_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>

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
#define debug(M, ...)                \
  fprintf(stderr,                    \
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
