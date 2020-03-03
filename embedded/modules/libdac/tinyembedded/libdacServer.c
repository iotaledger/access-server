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
 * \file libdacServer.c
 * \brief
 * Implementation of server side dac authentication
 *
 * @Author Dejan Nedic, Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 * 06.12.2018. Implemented ed25519 signature algorithm
 ****************************************************************************/

/////////////////
/// Includes
/////////////////
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dacdbg.h"

#include "libauthdac.h"

#include "libdac_internal.h"
#include "libdacUtils.h"


/////////////////////////////////////
/// Macros and defines
/////////////////////////////////////

/* AUTH_STAGES */
#define AUTH_ERROR   0
#define AUTH_INIT    1
#define AUTH_COMPUTE 2
#define AUTH_VERIFY  3
#define AUTH_FINISH  4
#define AUTH_DONE    5

//////////////////////////////////////////////////////////////////
/// Server authantication function declarations and definitions
//////////////////////////////////////////////////////////////////



/* AUTH_STAGES */int authServerInit(dacSession_t *session)
{
	int next_stage = AUTH_ERROR;

	unsigned char public[PUBLIC_KEY_L];
	unsigned char private[PRIVATE_KEY_L] ;
	crypto_sign_keypair(public,private);
	memcpy(getInternalPublic_key(session), public, PUBLIC_KEY_L);
	memcpy(getInternalPrivate_key(session), private, PRIVATE_KEY_L);
	memcpy(getInternalID_V(session) , "server", IDENTIFICATION_STRING_L);

//dn//     getInternalSeq_num(session) = 1;
    getInternalSeq_num_encrypt(session) = 1;
    getInternalSeq_num_decrypt(session) = 1;

	next_stage = AUTH_COMPUTE;

	return next_stage;
}

/* Stage 1 - Compute DH Client parameters
 *
 * Server receives e from Client.
 * Server generates  y.
 * Server calculates f = gy mod p.
 * Server computes K = ey mod p
 * Server computes H  = hash( Vc || Vs || Ks || e || f || K )
 * Server computes signature s = sign( sks, H )
 * Server sends ( Ks || f || s )
 *
 * Server receives ( Kc || sc )
 * Server computes Hc = hash( Vc || Vs || Kc || e || f || K )
 * Server verifies the signature sc on Hc
 *
 * */

/* AUTH_STAGES */int authServerCompute(dacSession_t *session)
{
	int next_stage = AUTH_ERROR;
	unsigned char Vc[] = {"client"};
	unsigned char s_signed[SIGNED_MESSAGE_L];

	int SIZE_OF_WRITE_BUFFER = PUBLIC_KEY_L + DH_PUBLIC_L + SIGNED_MESSAGE_L;
	unsigned char writeBuffer[SIZE_OF_WRITE_BUFFER];
	unsigned char received_dh_public[DH_PUBLIC_L];

	unsigned char readBuffer[PUBLIC_KEY_L + SIGNED_MESSAGE_L];
	unsigned char *client_public_key;
	unsigned char *signature;

	// Server receives e from Client.
	int read_message = session->f_read(session->ext, received_dh_public, DH_PUBLIC_L);

	// Server generates y and computes f
	int keys_generated = dh_generate_keys(session);

	// Server computes K = ey mod p
	int secret_computed = dh_compute_secret_K(session, received_dh_public);

	// Server computes H  = hash( Vc || Vs || Ks || e || f || K )
	int h_computed = compute_session_identifier_H(getInternalExchange_hash(session), Vc, getInternalID_V(session), getInternalPublic_key(session), received_dh_public, getInternalDH_public(session), getInternalSecret_K(session));

	// Server computes signature s = sign( sks, H )
	int signature_computed = compute_signature_s(s_signed, session, getInternalExchange_hash(session));
	if(signature_computed == 1)
	{
		Dlog_printf("\nERROR signing failed\n");

		return next_stage;
	}

	// Server sends ( Ks || f || s )
	concatinate_strings(writeBuffer, getInternalPublic_key(session), PUBLIC_KEY_L, getInternalDH_public(session), DH_PUBLIC_L);
	concatinate_strings(writeBuffer + PUBLIC_KEY_L + DH_PUBLIC_L, NULL, 0, s_signed, SIGNED_MESSAGE_L);
//	Dlog_printf("\nServer sends Ks || f || s to client\n");
	int write_message = session->f_write(session->ext, writeBuffer, SIZE_OF_WRITE_BUFFER);

	// Server receives ( Kc || sc )
	int read_second_message = session->f_read(session->ext, readBuffer, PUBLIC_KEY_L + SIGNED_MESSAGE_L);
	client_public_key = readBuffer;
	signature = readBuffer + PUBLIC_KEY_L;

	// Server verifies that Kc is the public key for Client
	int key_verified = session->f_verify(client_public_key, PUBLIC_KEY_L);

	// Server computes Hc = hash( Vc || Vs || Kc || e || f || K )
	int hc_computed = compute_session_identifier_H(getInternalExchange_hash2(session), Vc, getInternalID_V(session), client_public_key, received_dh_public, getInternalDH_public(session), getInternalSecret_K(session));

	// Server verifies the signature sc on Hc
	int signature_verified = verify_signature(signature, client_public_key, getInternalExchange_hash2(session));

	if( (read_message == DH_PUBLIC_L) &&
		(keys_generated == 0) &&
		(secret_computed == 0) &&
		(h_computed == 0) &&
		(signature_computed == 0) &&
		(write_message == SIZE_OF_WRITE_BUFFER) &&
		(read_second_message == PUBLIC_KEY_L + SIGNED_MESSAGE_L) &&
		(key_verified == 0) &&
		(hc_computed == 0) &&
		(signature_verified == 0) )
	{
		next_stage = AUTH_FINISH;
		Dlog_printf("\nAuthentication successful\n");
	}
	else
	{
		Dlog_printf("\nERROR authentication failed\n");
	}

	return next_stage;
}

/* Stage 2 - Finish authentication
 *
 * Server generates AES keys.
 *
 * */

/* AUTH_STAGES */int authServerFinish(dacSession_t *session)
{
	int next_stage = AUTH_ERROR;
	//debug("\nGenrating encryption and authorisation keys\n");

	int generated = 0;

	// Server generates AES keys.
	generated += generate_enc_auth_keys(getInternalIV_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'B');
	generated += generate_enc_auth_keys(getInternalIV_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'A');
	generated += generate_enc_auth_keys(getInternalDecryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'C');
	generated += generate_enc_auth_keys(getInternalEncryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'D');
	generated += generate_enc_auth_keys(getInternalIntegrity_key_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'E');
	generated += generate_enc_auth_keys(getInternalIntegrity_key_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'F');

	//AES_init_ctx_iv(&getInternalCtx_encrypt(session), getInternalEncryption_key(session), getInternalIV_encryption(session));
	   //AES_init_ctx_iv(&getInternalCtx_decrypt(session), getInternalDecryption_key(session), getInternalIV_decryption(session));

	if(generated == 0)
	{
		next_stage = AUTH_DONE;
		//debug("\nGenrating encryption and authorisation keys finished");
	}
	else
	{
		//debug("\nERROR genrating encryption and authorisation keys");
	}

	return next_stage;
}


/* DAC_ERRORS */int dacServerAuthenticate(dacSession_t *session)
{
	int ret = DAC_ERROR;

	int authStage = AUTH_INIT;

	while ((AUTH_DONE != authStage) && (AUTH_ERROR != authStage))
	{
		switch (authStage) {
			case AUTH_INIT:
				authStage = authServerInit(session);
				break;
			case AUTH_COMPUTE:
				authStage = authServerCompute(session);
				break;
			case AUTH_FINISH:
				authStage = authServerFinish(session);
				ret = 0;
				break;
			default:
				break;
		}
	}

	return ret;
}

/* DAC_ERRORS */int dacSendServer(dacSession_t *session, const unsigned char *data, unsigned short  data_len)
{
   return dacUtilsWrite(session, data, data_len);
}

/* DAC_ERRORS */int dacReceiveServer(dacSession_t *session, unsigned char **data, unsigned short  *data_len)
{
   return dacUtilsRead(session, data, data_len);
}

void dacReleaseServer(dacSession_t *session)
{

}

int dacServerSetOption(dacSession_t *session, const char *key, unsigned char *value)
{
	return dacUtilSetOption(session, key, value);
}
