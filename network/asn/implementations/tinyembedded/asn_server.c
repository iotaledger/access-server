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
 * \file asn_server.c
 * \brief
 * Implementation of server side ASN authentication
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
#include "asn_debug.h"
#include "asn_internal.h"
#include "asn_utils.h"


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



int authServerInit(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

	unsigned char public[PUBLIC_KEY_L];
	unsigned char private[PRIVATE_KEY_L] ;
	crypto_sign_keypair(public,private);
	memcpy(getInternalPublic_key(session), public, PUBLIC_KEY_L);
	memcpy(getInternalPrivate_key(session), private, PRIVATE_KEY_L);
	memcpy(getInternalID_V(session) , "server", IDENTIFICATION_STRING_L);

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

int authServerCompute(asnSession_t *session)
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
	int keys_generated = asnUtils_dh_generate_keys(session);

	// Server computes K = ey mod p
	int secret_computed = asnUtils_dh_compute_secret_K(session, received_dh_public);

	// Server computes H  = hash( Vc || Vs || Ks || e || f || K )
	int h_computed = asnUtils_compute_session_identifier_H(getInternalExchange_hash(session), Vc, getInternalID_V(session), getInternalPublic_key(session), received_dh_public, getInternalDH_public(session), getInternalSecret_K(session));

	// Server computes signature s = sign( sks, H )
	int signature_computed = asnUtils_compute_signature_s(s_signed, session, getInternalExchange_hash(session));
	if(signature_computed == 1)
	{
		Dlog_printf("\nERROR signing failed\n");

		return next_stage;
	}

	// Server sends ( Ks || f || s )
	asnUtils_concatenate_strings(writeBuffer, getInternalPublic_key(session), PUBLIC_KEY_L, getInternalDH_public(session), DH_PUBLIC_L);
	asnUtils_concatenate_strings(writeBuffer + PUBLIC_KEY_L + DH_PUBLIC_L, NULL, 0, s_signed, SIGNED_MESSAGE_L);

	int write_message = session->f_write(session->ext, writeBuffer, SIZE_OF_WRITE_BUFFER);

	// Server receives ( Kc || sc )
	int read_second_message = session->f_read(session->ext, readBuffer, PUBLIC_KEY_L + SIGNED_MESSAGE_L);
	client_public_key = readBuffer;
	signature = readBuffer + PUBLIC_KEY_L;

	// Server verifies that Kc is the public key for Client
	int key_verified = session->f_verify(client_public_key, PUBLIC_KEY_L);

	// Server computes Hc = hash( Vc || Vs || Kc || e || f || K )
	int hc_computed = asnUtils_compute_session_identifier_H(getInternalExchange_hash2(session), Vc, getInternalID_V(session), client_public_key, received_dh_public, getInternalDH_public(session), getInternalSecret_K(session));

	// Server verifies the signature sc on Hc
	int signature_verified = asnUtils_verify_signature(signature, client_public_key, getInternalExchange_hash2(session));

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

int authServerFinish(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

	int generated = 0;

	// Server generates AES keys.
	generated += asnUtils_generate_enc_auth_keys(getInternalIV_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'B');
	generated += asnUtils_generate_enc_auth_keys(getInternalIV_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'A');
	generated += asnUtils_generate_enc_auth_keys(getInternalDecryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'C');
	generated += asnUtils_generate_enc_auth_keys(getInternalEncryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'D');
	generated += asnUtils_generate_enc_auth_keys(getInternalIntegrity_key_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'E');
	generated += asnUtils_generate_enc_auth_keys(getInternalIntegrity_key_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'F');

	if(generated == 0)
	{
		next_stage = AUTH_DONE;
	}

	return next_stage;
}

int asnInternal_server_authenticate(asnSession_t *session)
{
	int ret = ASN_ERROR;

	int authStage = AUTH_INIT;

	while ((AUTH_DONE != authStage) && (AUTH_ERROR != authStage))
	{
		switch (authStage)
		{
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

int asnInternal_server_send(asnSession_t *session, const unsigned char *data, unsigned short  data_len)
{
   return asnUtils_write(session, data, data_len);
}

int asnInternal_server_receive(asnSession_t *session, unsigned char **data, unsigned short  *data_len)
{
   return asnUtils_read(session, data, data_len);
}

void asnInternal_release_server(asnSession_t *session)
{

}

int asnInternal_server_set_option(asnSession_t *session, const char *key, unsigned char *value)
{
	return asnUtils_set_option(session, key, value);
}
