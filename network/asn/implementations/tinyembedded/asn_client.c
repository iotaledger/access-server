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
 * \file asn_client.c
 * \brief
 * Implementation of client side ASN authentication
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
/// Client authantication function declarations and definitions
//////////////////////////////////////////////////////////////////

int authClientInit(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

    unsigned char public[PUBLIC_KEY_L];
    unsigned char private[PRIVATE_KEY_L];


    //generate private and public key on client side
    crypto_sign_keypair(public, private);

	memcpy(getInternalPublic_key(session), public, PUBLIC_KEY_L);
	memcpy(getInternalPrivate_key(session), private, PRIVATE_KEY_L);

	memcpy(getInternalID_V(session), "client", IDENTIFICATION_STRING_L);
	next_stage = AUTH_COMPUTE;
	return next_stage;
}


/* Stage 1 - DH parameter generation
 *
 * Client generates x and calculates e = gx mod p.
 * Client sends e to Server.
 *
 * */

int authClientGenerate(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

	// Client generates p, g, Vc and x and calculates e = gx mod p.
	int keys_generated = asnUtils_dh_generate_keys(session);

	// Client sends e to Server.
	int write_message = session->f_write(session->ext, getInternalDH_public(session), DH_PUBLIC_L);

    getInternalSeq_num_encrypt(session) = 1;
    getInternalSeq_num_decrypt(session) = 1;

	if((keys_generated == 0) && (write_message == DH_PUBLIC_L))
	{
		next_stage = AUTH_VERIFY;
	}

	return next_stage;
}


/* Stage 2 - Compute and verify Server parameters
 *
 * Client receives ( Ks || f || s )
 * Client computes K = fx mod p
 * Client computes H  = hash( Vc || Vs || Ks || e || f || K )
 * Client verifies the signature s on H
 * Client computes Hc = hash( Vc || Vs || Kc || e || f || K )
 * Client computes signature sc = sign( skc, Hc )
 * Client sends ( Kc || sc ) to Server
 *
 * */

int authClientVerify(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

	int SIZE_OF_READ_BUFFER = PUBLIC_KEY_L + DH_PUBLIC_L + SIGNED_MESSAGE_L; // size of Ks + f + s
	unsigned char readBuffer[SIZE_OF_READ_BUFFER];
	unsigned char *s_signed;
	unsigned char *received_dh_public;
	unsigned char *server_public_key;

	unsigned char Vs[IDENTIFICATION_STRING_L] ;
	memcpy(Vs , "server", IDENTIFICATION_STRING_L);
	unsigned char signature[SIGNED_MESSAGE_L];
	unsigned char message[PUBLIC_KEY_L + SIGNED_MESSAGE_L];

	// Client receives ( Ks || f || s )
	ssize_t read_message = session->f_read(session->ext, readBuffer, SIZE_OF_READ_BUFFER);
    if(read_message != SIZE_OF_READ_BUFFER)
    {
        return AUTH_ERROR;
    }

	server_public_key = readBuffer;
	received_dh_public = readBuffer + PUBLIC_KEY_L;
	s_signed = readBuffer + PUBLIC_KEY_L + DH_PUBLIC_L;

	// Client verifies that Ks is public key of the Server
	int key_verified = session->f_verify(received_dh_public, PUBLIC_KEY_L);

	// Client computes K = fx mod p
	int secret_computed = asnUtils_dh_compute_secret_K(session, received_dh_public);

	// Client computes H  = hash( Vc || Vs || Ks || e || f || K )
	int h_computed = asnUtils_compute_session_identifier_H(getInternalExchange_hash(session), getInternalID_V(session), Vs, server_public_key, getInternalDH_public(session), received_dh_public, getInternalSecret_K(session));

	// Client verifies the signature s on H
	int signature_verified = asnUtils_verify_signature(s_signed, server_public_key, getInternalExchange_hash(session));

	// Client computes Hc = hash( Vc || Vs || Kc || e || f || K )
	int computed_H =  asnUtils_compute_session_identifier_H(getInternalExchange_hash2(session), getInternalID_V(session), Vs, getInternalPublic_key(session), getInternalDH_public(session), received_dh_public, getInternalSecret_K(session));

	// Client computes signature sc = sign( skc, Hc )
	int message_signed = asnUtils_compute_signature_s(signature, session, getInternalExchange_hash2(session));

	asnUtils_concatenate_strings(message, getInternalPublic_key(session), PUBLIC_KEY_L, signature, SIGNED_MESSAGE_L);
	ssize_t message_written = session->f_write(session->ext, message, PUBLIC_KEY_L + SIGNED_MESSAGE_L);

	if(	(read_message == SIZE_OF_READ_BUFFER) &&
		(key_verified == 0) &&
		(secret_computed == 0) &&
		(h_computed == 0) &&
		(signature_verified == 0) &&
		(computed_H == 0) &&
		(message_signed == 0) &&
		(message_written == PUBLIC_KEY_L + SIGNED_MESSAGE_L) )
	{
		next_stage = AUTH_FINISH;
	}
	else
	{
        printf("\n%d\n", read_message);
        printf("%d\n", key_verified);
        printf("%d\n", secret_computed);
        printf("%d\n", h_computed);
        printf("%d\n", signature_verified);
        printf("%d\n", computed_H);
        printf("%d\n", message_signed);
        printf("%d\n", message_written);
	}

	return next_stage;
}


/* Stage 3 - Finish authentication
 *
 * Client generates AES keys.
 * Waits for encryption / decryption tasks :)
 *
 * */

int authClientFinish(asnSession_t *session)
{
	int next_stage = AUTH_ERROR;

	int generated = 0;

	// Client generates AES keys.
	generated += asnUtils_generate_enc_auth_keys(getInternalIV_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'A');
	generated += asnUtils_generate_enc_auth_keys(getInternalIV_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'B');
	generated += asnUtils_generate_enc_auth_keys(getInternalEncryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'C');
	generated += asnUtils_generate_enc_auth_keys(getInternalDecryption_key(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'D');
	generated += asnUtils_generate_enc_auth_keys(getInternalIntegrity_key_encryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'E');
	generated += asnUtils_generate_enc_auth_keys(getInternalIntegrity_key_decryption(session), getInternalSecret_K(session), getInternalExchange_hash(session), 'F');

	if(generated == 0)
	{
		next_stage = AUTH_DONE;
	}

	return next_stage;
}

int asnInternal_client_authenticate(asnSession_t *session)
{
	int ret = ASN_ERROR;

	int authStage = AUTH_INIT;
	while ((AUTH_DONE != authStage) && (AUTH_ERROR != authStage))
	{
		switch (authStage) {
			case AUTH_INIT:
				authStage = authClientInit(session);
				break;
			case AUTH_COMPUTE:
				authStage = authClientGenerate(session);
				break;
			case AUTH_VERIFY:
				authStage = authClientVerify(session);
				break;
			case AUTH_FINISH:
				authStage = authClientFinish(session);
				ret = 0;
				break;
			default:
				break;
		}
	}

	return ret;
}

int asnInternal_client_send(asnSession_t *session, const unsigned char *data, unsigned short  data_len)
{
   return asnUtils_write(session, data, data_len);
}

int asnInternal_client_receive(asnSession_t *session, unsigned char **data, unsigned short  *data_len)
{
   return asnUtils_read(session, data, data_len);
}

void asnInternal_release_client(asnSession_t *session)
{

}

int asnInternal_client_set_option(asnSession_t *session, const char *key, unsigned char *value)
{
	return asnUtils_set_option(session, key, value);
}