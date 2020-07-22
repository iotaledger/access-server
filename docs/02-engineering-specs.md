# Engineering Specification

- [Summary](#summary)
- [Architecture](#architecture)
  * [Access Core Software Development Kit (ACSDK)](#access-core-software-development-kit--acsdk-)
    + [Access Core API](#access-core-api)
      - [Policy Administration Point](#policy-administration-point)
      - [Policy Enforcement Point](#policy-enforcement-point)
      - [Policy Information Point](#policy-information-point)
      - [Policy Decision Point](#policy-decision-point)
    + [Platform Plugins](#platform-plugins)
      - [Input](#input)
        * [PIP Plugins](#pip-plugins)
        * [PAP Plugins](#pap-plugins)
      - [Output](#output)
        * [PEP Plugins](#pep-plugins)
    + [Access Secure Network API](#access-secure-network-api)
      - [libCrypto module](#libcrypto-module)
      - [Tiny Embedded module](#tiny-embedded-module)
      - [Server authentication key exchange](#server-authentication-key-exchange)
      - [Client public key authentication protocol](#client-public-key-authentication-protocol)
      - [Data encryption, decryption and validation](#data-encryption--decryption-and-validation)
  * [Access Core Server Reference Implementation (ACSRI)](#access-core-server-reference-implementation--acsri-)
    + [Access Actor](#access-actor)
    + [Wallet Actor](#wallet-actor)
    + [Network Actor](#network-actor)
    + [Application Supervisor](#application-supervisor)
  * [Access Policy](#access-policy)
  * [Access Request Protocol](#access-request-protocol)
  * [Policy Update Protocol](#policy-update-protocol)
    + [Policy Validation](#policy-validation)
- [Programming Language](#programming-language)
- [Access Core API](#access-core-api-1)
  * [Policy Administration Point](#policy-administration-point-1)
    + [Callback Functions (PAP Plugin)](#callback-functions--pap-plugin-)
    + [Internal Functions](#internal-functions)
    + [External API Functions](#external-api-functions)
    + [Policy Enforcement Point](#policy-enforcement-point-1)
    + [Policy Information Point](#policy-information-point-1)
    + [Policy Decision Point](#policy-decision-point-1)
  * [Access Secure Network API](#access-secure-network-api-1)

## Summary

This document contains technical information about **Access Server**. It is meant to guide engineers willing to collaborate with the project.

Apart from Access Server, the Access framework also includes the [Access Client](https://github.com/iotaledger/access-mobile), which is used to create the user experience for Policy Creation and Access Requests.

## Architecture

The Figure below demonstrates the conceptual relationship between different Access components.

It can be divided in 4 stacked layers:
- Application Layer
- Actor Layer
- API Layer
- Portability Layer

![drawing](/docs/images/access_structure.png)

The **Portability Layer** implements platform-dependent code. Anything related to drivers, Operating Systems and base libraries.

The **API Layer** implements platform-agnostic code. It's where Access Core, Access Secure Network, and IOTA functionality are implemented.

The **Actor Layer** is where different functionality gets implemented as [Actors](https://en.wikipedia.org/wiki/Actor_model) abstractions. It's where API calls are put together into cohesive blocks of specific functionality.

The **Application Layer** is where the Supervisor works as the main orchestrator that makes all Actors interact with each other. User Configurations are set in place, threads are initiated, and Actors are set up.

Together, the Portability and API Layers form the **Access Core Software Development Kit**.

Together, the Actor and Application Layers form the **Access Server Reference Implementation**.

###  Access Core Software Development Kit (ACSDK)

#### Access Core API
The Access Core API is divided into 4 different modules (and a few submodules):
- Policy Administration Point (PAP)
- Policy Information Point (PIP)
- Policy Enforcement Point (PEP)
- Policy Decision Point (PDP)

![drawing](/docs/images/pxp.png)

##### Policy Administration Point

**PAP** is used for managing updates to policies and determining which policies apply to what requests. The API expects callback functions to be registered as **PAP Plugins**.

##### Policy Enforcement Point

**PEP** routes the access request to the PDP for decision making, and acts on the received decision as appropriate within the system context. The API expects callback functions to be registered as **PEP Plugins**.

##### Policy Information Point

**PIP** coordinates the determination of attribute values used by the PDP. Attribute values are information collected from the outside world, such as IOTA transactions, sensor data or network traffic. The API expects callback functions to be registered as **Data Acquisition Plugins**.

##### Policy Decision Point

**PDP** is responsible for calculating the output for access requests. The API only consumed internally by other Core API modules.

On action request, Policy Enforcement Point (PEP) requests decision from Policy Decision Point (PDP), providing a `policyID` from the request. Based on the requested `policyID`, PDP  requests policy from the policy store and, if response is valid policy, it proceeds with calculation of decision. For both `policy goc` and `policy doc`, PDP calculates result of the policy, `true` or `false`, and then combines those results into decision, which can be one of four defined values: `grant`, `deny`, `conflict` or `undef`, and returns this result to PEP.

![drawing](/docs/images/pdp.png)

Calculation of `policy goc` and `policy doc` are modules that recursively solve every operation in the policy and return result at the end. The module is executed recursively as long as attributes in the attribute list of the policy contain predefined operations.

![drawing](/docs/images/pdp2.png)

#### Platform Plugins
Since IOTA Access can be used for different use cases, the underlying hardware used for resolving access decisions or for acquisition of data can take many different shapes and forms.

Platform Plugins are used to allow a platform-agnostic approach for different Access implementations.

Platform Plugins are written as callback functions. They talk to hardware drivers and perform system calls, while respecting the specifications imposed by their correspondent Access Core API modules.

Platform Plugins can be divided into Input and Output categories:

##### Input
###### PIP Plugins
PIP Plugins are used by PIP to gather information (Attributes) about the external environment.

For example, a Wallet-based PIP Plugin is used to verify whether an IOTA transaction was indeed performed. That allows PDP to calculate whether to grant access to the device renter.

Another example is of a GPS-based PIP Plugin that checks device location and uses this Attribute as input for PDP to make its decisions.

###### PAP Plugins
Policies need to be stored on the device in a non-volatile manner.

Embedded Systems can have different forms of permanent storage. It can be eMMC, SSD, SATA, USB, or many other examples. This variability implies the need for a modular approach for Policy Storage.

PAP Plugins are used by PAP to read and write Policies from non-volatile memory.

##### Output
###### PEP Plugins
PEP Plugins are used by PEP to enforce actions in the physical world. They are the main interface for actuators that need to behave in accordance to the Access being granted or denied.

For example, imagine a door that is controlled by a relay attached to the board's GPIO. The board is running the Access Server, and PDP has decided to deny the incoming Access Request. That means that the GPIO needs to set the relay such that the door remains locked.

#### Access Secure Network API
The Access Secure Network API is used to authenticate clients, ensure (off-Tangle) channel security and perform message validation.

The ASN authentication protocol is used to secure communication channel of the device. It is based on stripped version of SSH protocol. In order to be used in embedded system protocol must fulfill following requirements:
- Independent of physical communication layer
- Small memory footprint
- No OS dependencies
- Fast execution
- Off-Tangle communication between server and client

The ASN authentication protocol is designed as a module that can be used by both client and server applications. A top level ANS API unifies the usage and encapsulates the design, hiding the differences that client and server side facilitate and implement.

Client and server use-cases require different paths in authentication methodology. This leads to diversification of authentication steps for each case.

![drawing](/docs/images/ASN.png)

In order to meet requirements on different Embedded Systems, two versions of internal module realizations have been defined:

- **libCrypto**: module uses the derivative subset of [OpenSSL](https://www.openssl.org/), which is widely used, maintained and verified by the open source community.
- **Tiny embedded** module (based on 3rd party libs) is an even smaller realization of necessary functions required for ASN Authentication. Used when the target Embedded System's constrained resources are not able to support OpenSSL.
- HW acceleration is mentioned as an upgrade where applicable (depends of HW).

##### libCrypto module
libCrypto is the internal ASN realization based on OpenSSL. It is used in scenarios where Embedded resources are not scarce:

- **Diffie-Hellman key exchange**: used for generation and computation of the shared secret.
- **RSA**: used for signing and verification of the shared secrets.
- **AES256**: for message encryption and decryption.
- **SHA256**: for hashing during encryption key generation.
- **HMAC-SHA256**: used for data integrity during message exchange via Message Authentication Code (MAC).

##### Tiny Embedded module
Tiny Embedded is the internal ASN realization based on 3rd party libraries. It is used in scenarios where Embedded resources are scarce:
- **Curve25519 elliptic curve function**: to compute public key and shared secret for DH exchange.
- **ECDSA secp160r1**: for signing and signature validation.
- **SHA256**: for hashing.
- **AES256**: for message encryption and decryption.
- **HMAC-SHA256**: used for data integrity during message exchange via Message Authentication Code (MAC).

##### Server authentication key exchange

After physical connection is established, client generates a [Diffie-Hellman](https://mathworld.wolfram.com/Diffie-HellmanProtocol.html) (DH) private key. Based on the private key, client computes DH public key using following formula.

![drawing](/docs/images/formula.png)

Client sends its DH public key to the server running on the Target Device.

Server generates the DH key pair using the same algorithm used by client (dictated by the choice of either libCrypto or TinyEmbedded).

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

### Access Core Server Reference Implementation (ACSRI)
The Access Server Reference Implementation is meant to act as a reference for developers who want to write their own applications based on IOTA Access.

It runs on a Raspberry Pi, which means it is easily reproducible by Open Source Software Development Communities.

It showcases how to put together the different pieces from the Access Core SDK into a functional Application.

#### Access Actor
The Access Actor is mainly responsible for consuming the Access Core API. It puts together the different function calls such that Access Requests are properly addressed, and Policies are managed adequately.

#### Wallet Actor
The Wallet Actor is responsible for interacting with the IOTA Tangle. It initiates and checks IOTA Transactions while communicating with an IOTA Node.

The Wallet Actor is also responsible for managing the IOTA seed.

While the initial ASRI does not support a Secure Element for safe seed storage, this feature is in our roadmap for future integration. Probably, this will be achieved in a modular fashion via Secure Element Plugins for the Wallet Actor.

#### Network Actor
The Access Secure Network Actor works as a Daemon that listens for incoming Access Requests.

It is mainly responsible for consuming the Access Secure Network API in order to handle Ed25519 signatures, OpenSSL sessions and validating Access Requests.

#### Application Supervisor
The Application Supervisor works as the main orchestrator that makes all Actors interact with each other. User Configurations are set in place, threads and daemons are initiated, and Actors are set up.

### Access Policy
Access Policies are used by the device owner to express under which circumstances his devices will be accessed, and by whom.

The Access Policy Language is instance of the language [PBel](http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf) (pronounced “pebble”). A basic form of policy is captured in a rule. There are two simple types of rules from which more complex policies can be formed:

```
grant if cond
deny if !cond
```
where `cond` are logical expressions built from attributes, their values and comparisons, as well as logical operators.

For example, we may specify an access-control rule:

```
grant if (object == vehicle) && (subject == vehicle.owner.daughter) &&
         (action == driveVehicle) &&
         (0900 ≤ localTime) && (localTime ≤ 2000)
```

This rule implicitly refers to the request made by the daughter of the owner of the vehicle to drive that car. This rule applies only if the requested resource is that vehicle, the action is to drive that vehicle, and the requester is the daughter of the owner of that vehicle. In those circumstances, access is granted if the local time is between 9am and 8pm. The intuition is that this rule does not apply whenever its condition `cond` evaluates to `false`, including in cases in which the request is not of that type.

### Access Request Protocol

The Figure below shows a visual representation of the Access Request Protocol:

![drawing](/docs/images/request.png)

1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. Access Actor's PEP Listener and Network Actor's Request Listener Daemon loop.
4. Client and Server exchange keys and a secure connection is established. ASN Request Listener Daemon receives Access Request and forwards to Access Actor's PEP.
5. PEP requests decision from PDP.
6. PAP retrieves Policy from non-volatile memory via PAP Plugin.
7. PIP retrieves Attributes via Data Acquisition Plugin.
8. PDP decies actions + obligations.
9. PEP enforces actions + obligations via PEP Plugin.
10. Action is logged on the Tangle. Access and Network Actors go back to Listen mode.

### Policy Update Protocol
The Figure below shows a visual representation of the Policy Update Protocol:

![drawing](/docs/images/update.png)
1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. PAP sends Storage Checksum to Network Actor, who forwards it to the Tangle Policy Store (TPS). If ID sent by PAP matches with the Checksum of Policy List stored on the TPS, then TPS replies ok (nothing to update). If the ID differs, the TPS replies with Policy List (update required).
4. PAP Updater compares the received Policy List with all Policies stored locally. This is done via PAP Plugin.
5. PAP initiates the request for the Policies that differ. Request is relayed to Network Actor, who forwards it to TPS. TPS fetches Policy from IOTA Permanode and replies back.
6. PAP parses and validates the new Policies it receives.
7. PAP stores new Policies via PAP Plugin. PAP goes back to Checksum poll mode.

Note: The protocol described in this section will eventually evolve into a different approach. The entity called `Tangle Policy Store` is a Cloud Server that was inherited by the Legacy Design of early FROST implementations. Eventually this centralized entity will be completely removed and Policy Updates will be done via interactions between Access Actor and Wallet Actor, where the Wallet communicates with an IOTA Permanode.

#### Policy Validation
Policy Validation is the process where Policy structure is checked in order to:
  - Validate JSON.
  - Eliminate reduncancy of a binary circuit.
  - Limit values checks.
  - Check for suspicious conditions.

## Programming Language

IOTA Access Server is designed to run on Embedded Systems and IoT devices. Thus, it is imperative that it is implemented in the C programming language ([C99](http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf)), in order to ensure compatibility with these target devices.

[CMake](https://cmake.org/) v3.11 is used as Build System.

[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) is used to ensure that the codebase is formatted under the same style. [GNU  style](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#configurable-format-style-options) is used with a few modifications, namely:

```
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
BraceWrapping:
  IndentBraces: false
BreakBeforeBraces: Custom
IndentCaseLabels: true
IndentWidth: 4
SpaceBeforeParens: ControlStatements

```

## Access Core API

### Policy Administration Point

#### Callback Functions (PAP Plugin)
---
```
typedef bool (*put_fn)(char* policy_id, PAP_policy_object_t policy_object, PAP_policy_id_signature_t policy_id_signature, PAP_hash_functions_e hash_fn);
```
<!--
ToDo: write description
-->
xxx

---
```
typedef bool (*get_fn)(char* policy_id, PAP_policy_object_t *policy_object, PAP_policy_id_signature_t *policy_id_signature, PAP_hash_functions_e *hash_fn);
```
<!--
ToDo: write description
-->
xxx

---
```
typedef bool (*has_fn)(char* policy_id);

```
<!--
ToDo: write description
-->
xxx

---
```
typedef bool (*del_fn)(char* policy_id);
```
<!--
ToDo: write description
-->
xxx

#### Internal Functions
---
```
static void get_public_key_from_user(char *pk);
```
<!--
ToDo: write description
-->
xxx

---
```
static int normalize_JSON_object(char *json_object, int object_len, char **json_object_normalized);
```
<!--
ToDo: write description
-->
xxx

---
```
static void get_SHA256_hash(char *msg, int msg_len, char *hash_val);
```
<!--
ToDo: write description
-->
xxx

#### External API Functions
---
```
PAP_error_e PAP_init(void);
```
Initialize PAP module.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

---
```
PAP_error_e PAP_term(void);
```
Terminate PAP module.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

---
```
PAP_error_e PAP_register_callbacks(put_fn put, get_fn get, has_fn has, del_fn del);
```
Register callbacks for PAP Plugin.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

Parameters:
- `put_fn put`: Callback for writing policy data to storage.
- `get_fn get`: Callback for reading policy data from storage.
- `has_fn has`: Callback for checking if the policy is in the storage.
- `del_fn del`: Callback for deleting policy data from the storage.

---
```
PAP_error_e PAP_unregister_callbacks(void);
```
Unregister callbacks for PAP Plugin.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

---
```
PAP_error_e PAP_add_policy(char *signed_policy, int signed_policy_size);
```
Add new policy from cloud to embedded storage.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

Parameters:
- `char *signed_policy`: Signed policy string buffer.
- `int signed_policy_size`: Size of the signed policy string buffer.

---
```
PAP_error_e PAP_get_policy(char *policy_id, int policy_id_len, PAP_policy_t *policy);
```
Acquire policy data in order to make decision upon the request.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

Parameters:
- `char *policy_id`: Policy ID as a string.
- `int policy_id_len`: Length of the policy ID string.
- `PAP_policy_t *policy`: Requested policy data.

---
```
bool PAP_has_policy(char *policy_id, int policy_id_len);
```
Check if the policy is already stored.

Return values:
- 0: policy is not stored.
- 1: policy is stored.

Parameters:
- `char *policy_id,`: Policy ID as a string.
- `int policy_id_len`: Length of the policy ID string.

---
```
PAP_error_e PAP_remove_policy(char *policy_id, int policy_id_len);
```
Delete policy from embedded storage.

Return values:
- `PAP_NO_ERROR`
- `PAP_ERROR`

Parameters:
- `char *policy_id,`: Policy ID as a string.
- `int policy_id_len`: Length of the policy ID string.

#### Policy Enforcement Point

<!--
ToDo: add link to Doxygen here
-->
xxx

#### Policy Information Point

<!--
ToDo: add link to Doxygen here
-->
xxx

#### Policy Decision Point

<!--
ToDo: add link to Doxygen here
-->
xxx

### Access Secure Network API
ASN API is designed to both facilitate the client and the server side of the ASN protocol. The only difference between client and server usage of the API is at the initialization stage.

Underlying implementation can be based on either `libCrypto` or `TinyEmbedded`.

---
```
int asnAuth_init_client(asnSession_t *session, void *ext)
```
Initialization function used by the client application to initialize asnAuth library for client usage.

Return values:
- 0: Ok
- 1: Error

Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.
- `void *ext`: pointer to user-defined memory (callback function data).

---
```
int asnAuth_init_server(asnSession_t *session, void *ext)
```
Initialization function used by the server application to initialize asnAuth library for server usage.

Return values:
- 0: Ok
- 1: Error

Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.
- `void *ext`: pointer to user-defined memory (callback function data).

---
```
int asnAuth_set_option(asnSession_t *session, const char *key, unsigned char *value)
```
Function used to setup internal options.
Return values:
- 0: Ok
- 1: Error


Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.
- `const char *key`: xxx
- `unsigned char *value`: xxx

---
```
int asnAuth_authenticate(asnSession_t *session)
```
Function performs the algorithm of authentication defined by ASN authentication protocol above.
Return values:
- 0: Ok
- 1: Error

Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.

---
```
int asnAuth_send(asnSession_t *session, const unsigned char *data, unsigned short *len)
```
The application calls this function to send a message. All encapsulation, encryption and verification is performed internally.

Return values:
- 0: Ok
- 1: Error

Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.
- `const unsigned char *data`: message content.
- `unsigned short *len`: message length.

---
```
int asnAuth_receive(asnSession_t *session, unsigned char **data, unsigned short *len)
```
The application calls this function to receive a message. All encapsulation, decryption and verification is performed internally. Raw data and length is returned.
Return values:
- 0: Ok
- 1: Error

Parameters:
- `asnSession_t *session`: pointer to `asnSession` structure.
- `unsigned char **data`: message content.
- `unsigned short *len`: message length.

```
int asnAuth_release(asnSession_t *session)
```
The function is called when the `asnSession_t` structure is no longer needed. All internal memory is deallocated and the structure cannot be used anymore.
Return values:
- 0: Ok
- 1: Error

Parameters:
- pointer to `asnSession` structure
