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
 * \file asn_auth.c
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

#include "asn_debug.h"
#include "asn_auth.h"
#include "asn_internal.h"

static asnStruct_t internal;

/* ASN_ERRORS */static int asnAuth_init(asnSession_t *session, void *ext, int type)
{
   int ret = ASN_ERROR;

   if (NULL != session)
   {
      memset((void *) session, 0, sizeof(asnSession_t));

      getInternal(session) = &internal;//(asnStruct_t *) malloc(sizeof(asnStruct_t));

      if(NULL != getInternal(session))
      {
         memset((void *) getInternal(session), 0, sizeof(asnStruct_t));

         getInternalType(session) = type;
         session->ext = ext;

         ret = ASN_OK;
      }
   }

   return ret;
}

/* ASN_ERRORS */int asnAuth_init_client(asnSession_t *session, void *ext)
{
   return asnAuth_init(session, ext, ASN_TYPE_CLIENT);
}

/* ASN_ERRORS */int asnAuth_init_server(asnSession_t *session, void *ext)
{
   return asnAuth_init(session, ext, ASN_TYPE_SERVER);
}

/* ASN_ERRORS */int asnAuth_set_option(asnSession_t *session, const char *key, unsigned char *value)
{
	int ret = ASN_ERROR;

	//debug("asnAuth_set_option START");

	if (NULL != session)
	{
		if(ASN_TYPE_SERVER == getInternalType(session))
		{
			ret = asnInternal_server_set_option(session, key, value);
		}
		else if(ASN_TYPE_CLIENT == getInternalType(session))
		{
			ret = asnInternal_client_set_option(session, key, value);
		}
	}

	// debug("asnAuth_set_option END");

	return ret;
}

/* ASN_ERRORS */int asnAuth_authenticate(asnSession_t *session)
{
   int ret = ASN_ERROR;

   //debug("asnAuth_authenticate START");

   if (NULL != session)
   {
      if(ASN_TYPE_SERVER == getInternalType(session))
      {
         ret = asnInternal_server_authenticate(session);
      } else if(ASN_TYPE_CLIENT == getInternalType(session))
      {
         ret = asnInternal_client_authenticate(session);
      }
   }

  // debug("asnAuth_authenticate END");

   return ret;
}

/* ASN_ERRORS */int asnAuth_send(asnSession_t *session, const unsigned char *data, unsigned short  len)
{
   int ret = ASN_ERROR;

   //debug("asnAuth_send START");

   if ((NULL != session) && (NULL != data) && (len > 0))
   {
      /* Encrypt and send */
      if (ASN_TYPE_SERVER == getInternalType(session))
      {
         ret = asnInternal_server_send(session, data, len);
      } else if (ASN_TYPE_CLIENT == getInternalType(session))
      {
         ret = asnInternal_client_send(session, data, len);
      }
   }

   //debug("asnAuth_send END");

   return ret;
}

/* ASN_ERRORS */int asnAuth_receive(asnSession_t *session, unsigned char **data, unsigned short  *len)
{
   int ret = ASN_ERROR;

   //debug("asnAuth_receive START");

   if (NULL != session)
   {
      /* Decrypt and receive */
      if (ASN_TYPE_SERVER == getInternalType(session))
      {
         ret = asnInternal_server_receive(session, data, len);
      } else if (ASN_TYPE_CLIENT == getInternalType(session))
      {
         ret = asnInternal_client_receive(session, data, len);
      }
   }

  // debug("asnAuth_receive END");

   return ret;

}

/* ASN_ERRORS */int asnAuth_release(asnSession_t *session)
{
   int ret = ASN_ERROR;

   //debug("asnAuth_release START");

   if (NULL != getInternal(session))
   {
      if (ASN_TYPE_SERVER == getInternalType(session))
      {
         asnInternal_release_server(session);
      } else if (ASN_TYPE_CLIENT == getInternalType(session))
      {
         asnInternal_release_client(session);
      }

//      free((void *) getInternal(session));
      getInternal(session) = NULL;

      ret = ASN_OK;
   }

   if (NULL != session)
   {
      session = NULL;

      ret = ASN_OK;
   }



   //debug("asnAuth_release END");

   return ret;
}
