# Access Authentication API

The Access Auth API is used for Off-Tangle Authentication between Clients and Severs.
It comes in two flavours:
- RSA flavour
  - [NIST FIPS PUB 800-56A](https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-56ar.pdf) Diffie-Hellman (**DH**) key exchange.
  - [NIST FIPS PUB 800-131A](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-131Ar2.pdf): Rivest-Shamir-Adleman (**RSA**) 2048 signature scheme.
- ECDSA flavour
  - [NIST SP 800-186](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-186-draft.pdf): Elliptic Curve Diffie-Hellman (**ECDH**) key exchange.
  - [NIST SP 800-186](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-186-draft.pdf): Elliptic Curve Digital Signature Algorithm (**ECDSA**) with secp160r1.

Both flavours share the following cryptographic primitives:
- [NIST FIPS PUB 198-1](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.198-1.pdf): Keyed-Hash Message Authentication Code (**HMAC**) [**SHA-256**](https://www.cs.princeton.edu/~appel/papers/verif-sha.pdf).
- [NIST FIPS PUB 197](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf): Advanced Encryption Standard 256 (**AES256**).

The choice for Auth API flavour is done at CMake level.
Just `set(auth_flavour rsa)` or `set(auth_flavour ecdsa)` on the appropriate `CMakeLists.txt` to make your choice.

![drawing](/docs/images/auth.png)

## RSA Flavour
- key exchange: **DH**
- signing/verification: **RSA 2048**
- pros:
  - OpenSSL compatible.
  - keypair generated deterministically from IOTA seeds (IOTA 2.0) (via [dOpenSSL](https://github.com/bernardoaraujor/dopenssl)).
- cons:
  - RAM hungry.
- scenarios:
  - **SSL**/**TCP**/**IPv4**/**802.3** Wide Area Network (**WAN**) on **SoC HW-based MMU with 512Mb RAM** (Embedded Linux arm64).
  - **SSL**/**TCP**/**IPv4**/**802.3** World Wide Web (**WWW**) on **VPS** (Linux x86-64).

## ECDSA Flavour
- key exchange: **ECDH** with **Curve25519**
- signing/verification: **ECDSA** with **secp160r1**.
- pros:
  - RAM-light.
  - [Integrates with IOTA 1.5](https://github.com/Wollac/protocol-rfcs/blob/ed25519/text/0009-ed25519-signature-scheme/0009-ed25519-signature-scheme.md).
- cons:
  - Not Quantum robust.
-scenarios:
  - **SSL**/**TCP**/**IPv6**/**6LoWPAN** Bluetooth Low Energy (**BLE**) Network on **uC SW-based MMU with 256kb RAM** (FreeRTOS armv7-m).
  - **SSL**/**TCP**/**IPv4**/**802.11** Local Area Network (**LAN**) on **uC SW-based MMU with 520 KiB SRAM** (ZephyrOS esp32).

# Key Exchange

1. TCP/IP socket is established between Client and Server.
2. Client generates `client_DH_privkey`.
3. Client computes `client_DH_pubkey` based on `client_DH_privkey`.
4. Client sends `client_DH_privkey` to Server.
5. Server generates`server_DH_privkey` and `server_DH_pubkey` with the same algorithm.
6. Server computes DH-shared secret `K` and `H = hash (client_id || server_id || server_pubkey || client_DH_pubkey || server_DH_pubkey || K)`, where `client_id` and `server_id` are identification strings of Client and Server.
7. Server computes `signature = f(server_DH_privkey key, H)`.
8. Server sends `server_pubkey`, `server_DH_pubkey`, and `signature` to Client.
9. Client verifies the `server_pubkey`.
10. Client computes DH-shared secret `K`,`H = hash (client_id || server_id || server_pubkey || client_DH_pubkey || server_DH_pubkey || K)`.
11. Client verifies `signature`.

![drawing](/docs/images/key_exchange.png)

# Authentication Protocol

The `client_pubkey` authentication is executed after Server authentication key exchange (described above).

1. Client calculates `H = hash (client_id || server_id || client public__key || client_DH_pubkey || server_DH_pubkey || K)`, `signature = f(client_privkey, hash)`
2.Client sends `client_pubkey` + `signature` to Server.
3. Server verifies the `client_pubkey` and computes `H = hash (client_id || server_id || client_pubkey || client_DH_pubkey || server_DH_pubkey || K)`.
4. The server verifies `signature` on hash.

![drawing](/docs/images/client_key.png)

# Encryption, Decryption, Validation

When both Server and Client have shared secret `K` and hash `H`, following encryption keys can be generated:
- Initial IV client to server: `hash(K | |H| | ”A”)`
- Initial IV server to client: `hash(K | |H| | ”B”)`
- Encryption key client to server: `hash(K | |H| | ”C”)`
- Encryption key server to client: `hash(K | |H| | ”D”)`
- Integrity key client to server: `hash(K | |H| | ”E”)`
- Integrity key server to client: `hash(K | |H| | ”F”)`

Where `"A"`, `"B"`, `...` , `"F"` are constants representing `ASCII` values of those characters.

On sender side, `packet_length` and `payload` are encrypted with key:
- `encrypted_packet = enc(key, packet_length || payload)`
- `integrity_packet = mac(key, sequence_number || encrypted_packet_length || encrypted_packet)`
- `transmit_packet = encrypted_data_length || encrypted_packet || integrity_packet`

On receiver side, the reverse operation is performed in order to decrypt the package. The integrity packet is used for message authentication validation.

- `receive_packet = encrypted_data_length || encrypted_packet || integrity_packet`
- `confirm_integrity = mac(key, sequence_number || encrypted_packet_length || encrypted_packet)`
- `decrypted_packet = dec(Key, encrypted_packet)`

Sizes `packet_length` and `encrypted_packet_length` are 2 bytes in big endian format and size of `sequence_number` is 1 byte.
