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
 * \file libdac_internal.h
 * \brief
 * Header file with internal function definitions for dac authentication.
 *
 * @Author Nikola Kuzmanovic
 *
 * \notes
 *
 * \history
 * 14.08.2018. Initial version.
 * 06.12.2018. Implemented ed25519 signature algorithm
 ****************************************************************************/
#ifndef LIBDAC_INTERNAL_H_
#define LIBDAC_INTERNAL_H_

#include <string.h>

#include "aes.h"
#include "curve25519-donna.h"

#include "sha256.h"

#include "libauthdac.h"

#include "apiorig.h"



//////////////////////////////////////////
// Macros and defines
//////////////////////////////////////////

/* DAC_ERRORS */
#define DAC_TYPE_CLIENT          (0)
#define DAC_TYPE_SERVER          (1)

#define DH_PUBLIC_L 32
#define DH_PRIVATE_L 32
#define DH_SHARED_SECRET_L 32
#define EXCHANGE_HASH_L  32
#define IDENTIFICATION_STRING_L 6
#define PUBLIC_KEY_L 32
#define PRIVATE_KEY_L 64
#define IV_KEY_L 16
#define ENCRYPTION_KEY_L 32
#define INTEGRITY_KEY_L 32
#define SIGNED_MESSAGE_L 96
#define MAC_HASH_L 32
#define HASH_OUTPUT_L 32

#define I_PAD (0x36)
#define O_PAD (0x5C)
#define SHA256_BLOCK_BYTES 64

#define getInternal(s)               ((s)->internal)
#define getInternalType(s)           (getInternal(s)->type)
#define getInternalDH_public(s)      (getInternal(s)->dh_public)
#define getInternalDH_private(s)     (getInternal(s)->dh_private)
#define getInternalPublic_key(s)     (getInternal(s)->public_key)
#define getInternalPrivate_key(s)    (getInternal(s)->private_key)
#define getInternalExchange_hash(s)       (getInternal(s)->exchange_hash)
#define getInternalExchange_hash2(s)      (getInternal(s)->exchange_hash2)
#define getInternalSecret_K(s)       (getInternal(s)->secret_K)
#define getInternalID_V(s)           (getInternal(s)->identification_V)
#define getInternalSeq_num_decrypt(s)        (getInternal(s)->sequence_number_encrypt)
#define getInternalSeq_num_encrypt(s)        (getInternal(s)->sequence_number_decrypt)
#define getInternalCtx_encrypt(s)    (getInternal(s)->ctx_encrypt)
#define getInternalCtx_decrypt(s)    (getInternal(s)->ctx_decrypt)

#define getInternalIV_encryption(s)             (getInternal(s)->keys.IV_encryption)
#define getInternalIV_decryption(s)             (getInternal(s)->keys.IV_decryption)
#define getInternalEncryption_key(s)            (getInternal(s)->keys.encryption_key)
#define getInternalDecryption_key(s)            (getInternal(s)->keys.decryption_key)
#define getInternalIntegrity_key_encryption(s)  (getInternal(s)->keys.integrity_key_encryption)
#define getInternalIntegrity_key_decryption(s)  (getInternal(s)->keys.integrity_key_decryption)

//////////////////////////////////////////
// Structure definitions
//////////////////////////////////////////

typedef struct ea_keys {
	unsigned char IV_encryption[IV_KEY_L];
	unsigned char IV_decryption[IV_KEY_L];
	unsigned char encryption_key[ENCRYPTION_KEY_L];
	unsigned char decryption_key[ENCRYPTION_KEY_L];
	unsigned char integrity_key_encryption[INTEGRITY_KEY_L];
	unsigned char integrity_key_decryption[INTEGRITY_KEY_L];
} ea_keys_t;

struct dacStruct {
	int type; /*DAC_TYPE_X*/

	unsigned char dh_public[DH_PUBLIC_L];
	unsigned char dh_private[DH_PRIVATE_L];

	unsigned char secret_K[DH_SHARED_SECRET_L];

	unsigned char exchange_hash[EXCHANGE_HASH_L];
	unsigned char exchange_hash2[EXCHANGE_HASH_L];

	unsigned char identification_V[IDENTIFICATION_STRING_L];

	unsigned char public_key[PUBLIC_KEY_L];
	unsigned char private_key[PRIVATE_KEY_L];

	AES_ctx_t ctx_encrypt;
	AES_ctx_t ctx_decrypt;

	ea_keys_t keys;

	unsigned char sequence_number_encrypt;
	unsigned char sequence_number_decrypt;
};

//////////////////////////////////////////
// Function declarations and definitions
//////////////////////////////////////////
/**
 * @fn      int dacServerAuthenticate(dacSession_t *session)
 *
 * @brief   Function that executes steps of server side dac authentication.
 *
 * @param   session    Server session.
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacServerAuthenticate(dacSession_t *session);
/**
 * @fn      int dacClientAuthenticate(dacSession_t *session)
 *
 * @brief   Function that executes steps of client side dac authentication.
 *
 * @param   session    Client session.
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacClientAuthenticate(dacSession_t *session);
/**
 * @fn      int dacSendServer(dacSession_t *session, const unsigned char *msg, unsigned short msg_length)
 *
 * @brief   Function that is used to send message from server to client.
 *
 * @param   session     Server session
 * @param   msg         Message to send
 * @param   msg_length  Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacSendServer(dacSession_t *session, const unsigned char *msg, unsigned short msg_length);
/**
 * @fn      int dacSendClient(dacSession_t *session, const unsigned char *data, unsigned short  data_len)
 *
 * @brief   Function that is used to send message from client to server.
 *
 * @param   session     Client session
 * @param   data_len    Message to send
 * @param   data_len    Message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacSendClient(dacSession_t *session, const unsigned char *data, unsigned short  data_len);
/**
 * @fn      int dacReceiveServer(dacSession_t *session, unsigned char **msg, unsigned short  *msg_length)
 *
 * @brief   Function that is used to receive and read message from client.
 *
 * @param   session     Server session
 * @param   msg         Address of buffer for received message
 * @param   msg_length  Received message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacReceiveServer(dacSession_t *session, unsigned char **msg, unsigned short  *msg_length);
/**
 * @fn      int dacReceiveClient(dacSession_t *session, unsigned char **msg, unsigned short  *msg_length)
 *
 * @brief   Function that is used to receive and read message from server.
 *
 * @param   session     Client session
 * @param   msg         Address of buffer for received message
 * @param   msg_length  Received message length
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacReceiveClient(dacSession_t *session, unsigned char **msg, unsigned short  *msg_length);

/**
 * @fn      void dacReleaseServer(dacSession_t *session)
 *
 * @brief   Function that releases memory alocated for server session.
 *
 * @param   session     Server session
 */
void dacReleaseServer(dacSession_t *session);
/**
 * @fn      void dacReleaseClient(dacSession_t *session)
 *
 * @brief   Function that releases memory alocated for client session.
 *
 * @param   session     Client session
 */
void dacReleaseClient(dacSession_t *session);

/**
 * @fn      /int dacServerSetOption(dacSession_t *, const char *, unsigned char *)
 *
 * @brief   Function that releases memory alocated for server session.
 *
 * @param   session     Server session
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacServerSetOption(dacSession_t *, const char *, unsigned char *);

/**
 * @fn      int dacClientSetOption(dacSession_t *, const char *, unsigned char *)
 *
 * @brief   Function that releases memory alocated for client session.
 *
 * @param   session     Client session
 *
 * @return  0 if it succeeds, 1 if it fails.
 */
int dacClientSetOption(dacSession_t *, const char *, unsigned char *);

#endif /* LIBDAC_INTERNAL_H_ */
