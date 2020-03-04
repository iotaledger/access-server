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
 * \file libdacUtils.c
 * \brief
 * Implementation of key exchange and encryption key computation
 *
 * @Author Dejan Nedic, Milivoje Knezevic
 *
 * \notes
 *
 * \history
 * 31.07.2018. Initial version.
 * 06.12.2018. Implemented ed25519 signature algorithm
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#include "libdac_internal.h"
#include "libdacUtils.h"

//////////////////////////////////////
///
//////////////////////////////////////

int dh_generate_keys(dacSession_t *session)
{
	static const unsigned char basepoint[DH_PRIVATE_L] = {9};
	int r = rand();

	memset(getInternalDH_private(session), r, DH_PRIVATE_L);
	getInternalDH_private(session)[0] &= 248;
	getInternalDH_private(session)[31] &= 127;
	getInternalDH_private(session)[31] |= 64;

	curve25519_donna(getInternalDH_public(session), getInternalDH_private(session), basepoint);

	return 0;
}

int dh_compute_secret_K(dacSession_t *session,  const unsigned char *public_key)
{
	curve25519_donna(getInternalSecret_K(session), getInternalDH_private(session), public_key);

	return 0;
}

int hash(unsigned char *exchange_hash, unsigned char *message, int message_length)
{
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, message, message_length);
	sha256_final(&ctx, exchange_hash);

	return 0;
}

int compute_session_identifier_H(unsigned char *exchange_hash, unsigned char *Vc, unsigned char *Vs, unsigned char *K, unsigned char *c_public, unsigned char *s_public, unsigned char *secretK)
{
	int CONCATINATED_STRING_L = 2 * IDENTIFICATION_STRING_L + PUBLIC_KEY_L + 2 * DH_PUBLIC_L + DH_SHARED_SECRET_L;
	int CURENT_LENGTH = 0;
	unsigned char concatinated_string[CONCATINATED_STRING_L];

	for(int i = 0; i < IDENTIFICATION_STRING_L; i++)
		concatinated_string[i] = Vc[i];

	CURENT_LENGTH += IDENTIFICATION_STRING_L;
	for(int i = 0; i < IDENTIFICATION_STRING_L; i++)
		concatinated_string[CURENT_LENGTH + i] = Vs[i];

	CURENT_LENGTH += IDENTIFICATION_STRING_L;
	for(int i = 0; i < PUBLIC_KEY_L; i++)
		concatinated_string[CURENT_LENGTH + i] = K[i];

	CURENT_LENGTH += PUBLIC_KEY_L;
	for(int i = 0; i < DH_PUBLIC_L; i++)
		concatinated_string[CURENT_LENGTH + i] = c_public[i];

	CURENT_LENGTH += DH_PUBLIC_L;
	for(int i = 0; i < DH_PUBLIC_L; i++)
		concatinated_string[CURENT_LENGTH + i] = s_public[i];

	CURENT_LENGTH += DH_PUBLIC_L;
	for(int i = 0; i < DH_SHARED_SECRET_L; i++)
		concatinated_string[CURENT_LENGTH + i] = secretK[i];

	hash(exchange_hash, concatinated_string, CONCATINATED_STRING_L);

	return 0;
}

int generate_enc_auth_keys(unsigned char *hash, unsigned char *shared_secret_K, unsigned char *shared_H, char magic_letter)
{
	SHA256_CTX ctx;
	unsigned char contatinated_string[DH_SHARED_SECRET_L + EXCHANGE_HASH_L + 1];

	for(int i = 0; i < DH_SHARED_SECRET_L; i++)
		contatinated_string[i] = shared_secret_K[i];

	for(int i = 0; i < EXCHANGE_HASH_L; i++)
		contatinated_string[DH_SHARED_SECRET_L + i] = shared_H[i];

	contatinated_string[DH_SHARED_SECRET_L + EXCHANGE_HASH_L] = magic_letter;

	sha256_init(&ctx);
	sha256_update(&ctx, contatinated_string, sizeof(contatinated_string));
	sha256_final(&ctx, hash);

	return 0;
}

int compute_signature_s(unsigned char *sig, dacSession_t *session, unsigned char *hash)
{

	unsigned long long smlen;

	crypto_sign(sig,&smlen,hash,DH_SHARED_SECRET_L,getInternalPrivate_key(session));

	return 0;
}

int verify_signature(unsigned char *sig, unsigned char *public_key, unsigned char *hash)
{


	unsigned long long mlen;
	int ret = crypto_sign_open(hash,&mlen,sig,SIGNED_MESSAGE_L,public_key);

	return -ret;
}

int concatinate_strings(unsigned char *concatinatedString, unsigned char *str1, int str1_l, unsigned char * str2, int str2_l)
{
	for(int i = 0; i < str1_l; i++)
	{
		concatinatedString[i] = str1[i];
	}

	for(int i = 0; i < str2_l; i++)
	{
		concatinatedString[str1_l + i] = str2[i];
	}

	return 0;
}

int aes_encrypt(AES_ctx_t *ctx, unsigned char *message, int length)
{
	//debug("Encrypting message");
	AES_CBC_encrypt_buffer(ctx, message, length);
	//debug("Message encrypted");
//	print_key(message, length);

	return 0;
}

int aes_decrypt(AES_ctx_t *ctx, unsigned char *message, int length)
{
	//debug("Decrypting message");
	AES_CBC_decrypt_buffer(ctx, message, length);
	//debug("Message decrypted");
//	print_key(message, length);

	return 0;
}

void hmac_sha256(unsigned char *mac, unsigned char *integrityKey, uint16_t keyLength, unsigned char *message, uint32_t messageLength)
{
	SHA256_CTX ss;
	unsigned char kh[HASH_OUTPUT_L];
	unsigned char key[keyLength];
	unsigned char *internal_key;
	size_t internal_key_l = 0;

	memcpy(key, integrityKey, keyLength);

	if (keyLength > SHA256_BLOCK_BYTES) {
		sha256_init (&ss);
		sha256_update (&ss, key, keyLength);
		sha256_final (&ss, kh);

		internal_key = kh;
		internal_key_l = HASH_OUTPUT_L;
	}
	else
	{
		internal_key = key;
		internal_key_l = keyLength;
	}

	unsigned char kx[SHA256_BLOCK_BYTES];
	for (size_t i = 0; i < internal_key_l; i++) kx[i] = I_PAD ^ internal_key[i];
	for (size_t i = internal_key_l; i < SHA256_BLOCK_BYTES; i++) kx[i] = I_PAD ^ 0;

	sha256_init (&ss);
	sha256_update (&ss, kx, SHA256_BLOCK_BYTES);
	sha256_update (&ss, message, messageLength);
	sha256_final (&ss, mac);

	for (size_t i = 0; i < internal_key_l; i++) kx[i] = O_PAD ^ internal_key[i];
	for (size_t i = internal_key_l; i < SHA256_BLOCK_BYTES; i++) kx[i] = O_PAD ^ 0;

	sha256_init (&ss);
	sha256_update (&ss, kx, SHA256_BLOCK_BYTES);
	sha256_update (&ss, mac, HASH_OUTPUT_L);
	sha256_final (&ss, mac);
}

int dacUtilsWrite(dacSession_t *session, const unsigned char *msg, unsigned short messageLength)
{
    unsigned short encrypted_data_length = ((messageLength + 2 + 15) / 16 ) * 16; // determine size of encrypted data with padding
    unsigned short buffer_length = encrypted_data_length + MAC_HASH_L + 2 + 1; //2 bytes for encrypted data length 1 byte for sequence_number
    unsigned char mac[MAC_HASH_L];

    unsigned char buffer[buffer_length];

    buffer[0] = getInternalSeq_num_encrypt(session);
//    debug("\nSequence number: %d \n", buffer[0]);

    buffer[1] = ((encrypted_data_length >> 8));
//    printf("\nEncrypted data length f.b.: %d\n", buffer[1]);
    buffer[2] = (encrypted_data_length);
//    printf("\nEncrypted data length s.b.: %d\n", buffer[2]);
    buffer[3] = ((messageLength >> 8));
//    printf("\nData length f.b.: %d\n", buffer[3]);
    buffer[4] = messageLength;
//    printf("\nData length s.b.: %d\n", buffer[4]);

    for(int i = 0; i < messageLength; i++)
	{
		buffer[i + 5] = msg[i];
	}
    /*
    if((messageLength + 2) % 8 != 0)
    {
        addPadding(encrypted_data_length - messageLength - 2, buffer);
    }
    */
    for(int i = messageLength + 2; i < encrypted_data_length; i++)
    {
        buffer[i + 3] = 0;
    }

    aes_encrypt(&getInternalCtx_encrypt(session), buffer + 3, encrypted_data_length);

    hmac_sha256(mac, getInternalIntegrity_key_encryption(session), INTEGRITY_KEY_L, buffer, encrypted_data_length + 3);

	for(int i = 0; i < MAC_HASH_L; i++)
	{
		buffer[i + encrypted_data_length + 2 + 1] = mac[i];
	}

	int n = session->f_write(session->ext, buffer, buffer_length);

    //debug("Data sent");

    getInternalSeq_num_encrypt(session)++;


    if(n <= 0)
    {
        return 1;
    }

    return 0;
}

int dacUtilsRead(dacSession_t *session, unsigned char **msg, unsigned short *messageLength)
{

	unsigned short encrypted_data_length = 0;

	//    unsigned short msg_buffer_length = 0;// encrypted_data_length + MAC_HASH_L + 2 + 1;

	unsigned char sequence_number = 0;

	unsigned char received_mac[MAC_HASH_L];
	unsigned char mac[MAC_HASH_L];

	unsigned char buffer[5];
	unsigned char *encrypted_msg_buffer;

	session->f_read(session->ext, buffer, 3);

	sequence_number = buffer[0];

	if(sequence_number != getInternalSeq_num_decrypt(session))
	{
//		Dlog_printf("Received sequence number %d doesn't match expected: %d", sequence_number, getInternalSeq_num_decrypt(session));
		return 1;
	}
	encrypted_data_length = buffer[1];
	encrypted_data_length *= 256;
	encrypted_data_length += buffer[2];

	//Dlog_printf("\nEncrypted data length: %d\n", encrypted_data_length);

	encrypted_msg_buffer = malloc(encrypted_data_length + 3);

	if(NULL == encrypted_msg_buffer)
	{
		return 1;
	}

	encrypted_msg_buffer[0] = buffer[0];
	encrypted_msg_buffer[1] = buffer[1];
	encrypted_msg_buffer[2] = buffer[2];

	session->f_read(session->ext, encrypted_msg_buffer + 3, encrypted_data_length);

	session->f_read(session->ext, received_mac, MAC_HASH_L);

	hmac_sha256(mac, getInternalIntegrity_key_decryption(session), INTEGRITY_KEY_L, encrypted_msg_buffer, encrypted_data_length + 3);
/*
	Dlog_printf("\n\nMAC: (%d)\n", MAC_HASH_L);
	for(int z = 0; z < MAC_HASH_L; z++)
	{
		Dlog_printf("0x%.02x, ",mac[z]);
	}
	Dlog_printf("\n");
*/
	if(memcmp(mac, received_mac, MAC_HASH_L) == 0)
	{
//		Dlog_printf("Data integrity confirmed");

		aes_decrypt(&getInternalCtx_decrypt(session), encrypted_msg_buffer + 3, encrypted_data_length);


		*messageLength = encrypted_msg_buffer[3];
		*messageLength *= 256;
		*messageLength += encrypted_msg_buffer[4];

//		Dlog_printf("\nmessageLength %d\n", *messageLength);

		int i = 0;
		unsigned char *ss = malloc(*messageLength);


		//     print_key(encrypted_msg_buffer + 5, *messageLength);

		if(NULL != ss)
		{
			for(i = 0; i < *messageLength; i++)
			{
				ss[i] = encrypted_msg_buffer[i + 5];
				//          debug("%02x", ss[i]);
			}
			free(encrypted_msg_buffer);
			*msg = ss;
			getInternalSeq_num_decrypt(session)++;

		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}

	return 0;
}

int dacUtilSetOption(dacSession_t *session, const char *key, unsigned char *value)
{
	int ret = DAC_ERROR;

	if((strlen(key) == 7) && (0 == memcmp(key, "private", 7)))
	{
		memcpy(getInternalPrivate_key(session), value, PRIVATE_KEY_L);
		ret = DAC_OK;
	}
	else if((strlen(key) == 6) && (0 == memcmp(key, "public", 6)))
	{
		memcpy(getInternalPublic_key(session), value, PUBLIC_KEY_L);
		ret = DAC_OK;
	}

	return ret;
}









