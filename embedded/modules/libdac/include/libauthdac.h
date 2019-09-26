/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
 * \project Decentralized Access Control
 * \file libdacauth.h
 * \brief
 *    Authentication module API
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 ****************************************************************************/

#ifndef LIBAUTHDAC_H
#define LIBAUTHDAC_H

//////////////////////////////////////////
// Include files
//////////////////////////////////////////

#include <sys/types.h>

#include "stdio.h"

//#include "lwip/arch.h"

//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* DAC_ERRORS */
#define DAC_OK    0
#define DAC_ERROR 1

#define AUTH_SERVER_PORT 9998

//////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif
typedef ssize_t f_dac_ext_t(void *, void *, unsigned short);

typedef int f_dac_key_verify(unsigned char *, int);

typedef struct dacStruct dacStruct_t;

typedef struct dacSession {
   dacStruct_t *internal;

   void *ext; /* External data structure */

   f_dac_ext_t *f_write;
   f_dac_ext_t *f_read;

   f_dac_key_verify *f_verify;

   int status;
} dacSession_t;

//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////

/* DAC_ERRORS */int dacInitClient(dacSession_t *, void *);
/* DAC_ERRORS */int dacInitServer(dacSession_t *, void *);

/* DAC_ERRORS */int dacSetOption(dacSession_t *, const  char *, unsigned char *);

/* DAC_ERRORS */int dacAuthenticate(dacSession_t *);

/* DAC_ERRORS */int dacSend(dacSession_t *, const unsigned char *, unsigned short);

/* DAC_ERRORS */int dacReceive(dacSession_t *, unsigned char **, unsigned short *);

/* DAC_ERRORS */int dacRelease(dacSession_t *);
#ifdef __cplusplus
};
#endif
#endif /* LIBAUTHDAC_H */
