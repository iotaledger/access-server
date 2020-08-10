#### Access Authentication API
The Access Authentication API is used to authenticate clients, ensure (off-Tangle) channel security and perform message validation.

The authentication protocol is used to secure communication channel of the device. It is based on stripped version of SSH protocol. In order to be used in embedded system protocol must fulfill following requirements:
- Independent of physical communication layer
- Small memory bartprint
- No OS dependencies
- Fast execution
- Off-Tangle communication between server and client

The authentication protocol is designed as a module that can be used by both client and server applications. A top level Authentication API unifies the usage and encapsulates the design, hiding the differences that client and server side facilitate and implement.

Client and server use-cases require different paths in authentication methodology. This leads to diversification of authentication steps for each case.

![drawing](/docs/images/auth.png)

In order to meet requirements on different Embedded Systems, two versions of internal module realizations have been defined:

- **OpenSSL**: module uses the derivative subset of [OpenSSL](https://www.openssl.org/), which is widely used, maintained and verified by the open source community.
- **Tiny embedded** module (based on 3rd party libs) is an even smaller realization of necessary functions required for Authentication. Used when the target Embedded System's constrained resources are not able to support OpenSSL.
- HW acceleration is mentioned as an upgrade where applicable (depends of HW).

##### OpenSSL module
The first option is the internal authentication realization based on OpenSSL. It is used in scenarios where Embedded resources are not scarce:

- **Diffie-Hellman key exchange**: used for generation and computation of the shared secret.
- **RSA**: used for signing and verification of the shared secrets.
- **AES256**: for message encryption and decryption.
- **SHA256**: for hashing during encryption key generation.
- **HMAC-SHA256**: used for data integrity during message exchange via Message Authentication Code (MAC).

##### Tiny Embedded module
Tiny Embedded is the internal authentication realization based on 3rd party libraries. It is used in scenarios where Embedded resources are scarce:
- **Curve25519 elliptic curve function**: to compute public key and shared secret for DH exchange.
- **ECDSA secp160r1**: for signing and signature validation.
- **SHA256**: for hashing.
- **AES256**: for message encryption and decryption.
- **HMAC-SHA256**: used for data integrity during message exchange via Message Authentication Code (MAC).

##### Server authentication key exchange

After physical connection is established, client generates a [Diffie-Hellman](https://mathworld.wolfram.com/Diffie-HellmanProtocol.html) (DH) private key. Based on the private key, client computes DH public key using following formula.

![drawing](/docs/images/formula.png)

Client sends its DH public key to the server running on the Target Device.

Server generates the DH key pair using the same algorithm used by client (dictated by the choice of either OpenSSL or TinyEmbedded).

Server computes the DH-shared secret `K` and hash `H = hash (client ID || server ID || server public key || client DH public key || server DH public key || shared secret K)`, where Client ID and server ID are identification strings of client and server.

Using its own private key, server also computes `signature = f(private key, H)`.

Server sends server public key, server DH public key, and signature to client.

Client first verifies the server's public key. It computes DH-shared secret `K`, hash `H = hash (client ID || server ID || server public key || client DH public key || server DH public key || shared secret K)` and verifies signature.

![drawing](/docs/images/key_exchange.png)

##### Client public key authentication protocol

The client public key authentication is executed after server authentication key exchange (described above).

Client calculates hash `H = hash (client ID || server ID || client public key || client DH public key || server DH public key || shared secret K)`, signature `S = f(client private key, hash)`, and sends public key + signature to server.

Server verifies the Client's public key and computes hash `H = hash (client ID || server ID || client public key || client DH public key || server DH public key || shared secret K)`. Then, the server verifies signature `S` on hash.

![drawing](/docs/images/client_key.png)

##### Data encryption, decryption and validation

When both server and client have shared secret `K` and hash `H`, following encryption keys can be generated:
- Initial IV client to server: `hash(K||H||”A”)`
- Initial IV server to client: `hash(K||H||”B”)`
- Encryption key client to server: `hash(K||H||”C”)`
- Encryption key server to client: `hash(K||H||”D”)`
- Integrity key client to server: `hash(K||H||”E”)`
- Integrity key server to client: `hash(K||H||”F”)`
"A", "B", ... "F" are constants representing ASCII values of those characters.

Encryption key is performed over packet length and payload:

`Encrypted_packet = enc(Key, packet_length||payload)`

`Integrity_packet = mac(key, sequence_number||encrypted_packet_length||encrypted_packet)`

`transmit_packet = encrypted_data_length||encrypted_packet||integrity_packet`

On the reception side, the reverse operation is performed in order to decrypt the package. The integrity packet is used for message authentication validation.

`receive_packet = encrypted_data_length||encrypted_packet||integrity_packet`

`confirm_integrity = mac(key, sequence_number||encrypted_packet_length||encrypted_packet)`

`decrypted_packet=dec(Key, encrypted_packet)`

Sizes `packet_length` and `encrypted_packet_length` are 2 bytes in big endian format and size of `sequence_number` is 1 byte.
