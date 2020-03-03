/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
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
 * \project Decentralized Access Control
 * \file libdacauth.c
 * \brief
 *    Central source file implementing the authentication module core
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 ****************************************************************************/

//#define NDEBUG

#include "dacdbg.h"
#include "libauthdac.h"
#include "libdac_internal.h"

static dacStruct_t internal;

/* DAC_ERRORS */int dacInit(dacSession_t *session, void *ext, int type)
{
   int ret = DAC_ERROR;

   if (NULL != session)
   {
      memset((void *) session, 0, sizeof(dacSession_t));

      getInternal(session) = &internal;//(dacStruct_t *) malloc(sizeof(dacStruct_t));

      if(NULL != getInternal(session))
      {
         memset((void *) getInternal(session), 0, sizeof(dacStruct_t));

         getInternalType(session) = type;
         session->ext = ext;

         ret = DAC_OK;
      }
   }

   return ret;
}

/* DAC_ERRORS */int dacInitClient(dacSession_t *session, void *ext)
{
   return dacInit(session, ext, DAC_TYPE_CLIENT);
}

/* DAC_ERRORS */int dacInitServer(dacSession_t *session, void *ext)
{
   return dacInit(session, ext, DAC_TYPE_SERVER);
}

/* DAC_ERRORS */int dacSetOption(dacSession_t *session, const char *key, unsigned char *value)
{
	int ret = DAC_ERROR;

	//debug("dacSetOption START");

	if (NULL != session)
	{
		if(DAC_TYPE_SERVER == getInternalType(session))
		{
			ret = dacServerSetOption(session, key, value);
		}
		else if(DAC_TYPE_CLIENT == getInternalType(session))
		{
			ret = dacClientSetOption(session, key, value);
		}
	}

	// debug("dacSetOption END");

	return ret;
}

/* DAC_ERRORS */int dacAuthenticate(dacSession_t *session)
{
   int ret = DAC_ERROR;

   //debug("dacAuthenticate START");

   if (NULL != session)
   {
      if(DAC_TYPE_SERVER == getInternalType(session))
      {
         ret = dacServerAuthenticate(session);
      } else if(DAC_TYPE_CLIENT == getInternalType(session))
      {
         ret = dacClientAuthenticate(session);
      }
   }

  // debug("dacAuthenticate END");

   return ret;
}

/* DAC_ERRORS */int dacSend(dacSession_t *session, const unsigned char *data, unsigned short  len)
{
   int ret = DAC_ERROR;

   //debug("dacSend START");

   if ((NULL != session) && (NULL != data) && (len > 0))
   {
      /* Encrypt and send */
      if (DAC_TYPE_SERVER == getInternalType(session))
      {
         ret = dacSendServer(session, data, len);
      } else if (DAC_TYPE_CLIENT == getInternalType(session))
      {
         ret = dacSendClient(session, data, len);
      }
   }

   //debug("dacSend END");

   return ret;
}

/* DAC_ERRORS */int dacReceive(dacSession_t *session, unsigned char **data, unsigned short  *len)
{
   int ret = DAC_ERROR;

   //debug("dacReceive START");

   if (NULL != session)
   {
      /* Decrypt and receive */
      if (DAC_TYPE_SERVER == getInternalType(session))
      {
         ret = dacReceiveServer(session, data, len);
      } else if (DAC_TYPE_CLIENT == getInternalType(session))
      {
         ret = dacReceiveClient(session, data, len);
      }
   }

  // debug("dacReceive END");

   return ret;

}

/* DAC_ERRORS */int dacRelease(dacSession_t *session)
{
   int ret = DAC_ERROR;

   //debug("dacRelease START");

   if (NULL != getInternal(session))
   {
      if (DAC_TYPE_SERVER == getInternalType(session))
      {
         dacReleaseServer(session);
      } else if (DAC_TYPE_CLIENT == getInternalType(session))
      {
         dacReleaseClient(session);
      }

//      free((void *) getInternal(session));
      getInternal(session) = NULL;

      ret = DAC_OK;
   }

   if (NULL != session)
   {
      session = NULL;

      ret = DAC_OK;
   }



   //debug("dacRelease END");

   return ret;
}
