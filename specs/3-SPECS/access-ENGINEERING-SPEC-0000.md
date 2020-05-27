# Engineering Specification

- [Frontmatter](#frontmatter)
- [Summary](#summary)
- [Logical System Design](#logical-system-design)
  * [Access Core Software Development Kit (ACSDK)](#access-core-software-development-kit--acsdk-)
    + [Access Core API](#access-core-api)
      - [Policy Administration Point](#policy-administration-point)
      - [Policy Enforcement Point](#policy-enforcement-point)
      - [Policy Information Point](#policy-information-point)
      - [Policy Decision Point](#policy-decision-point)
    + [Platform Plugins](#platform-plugins)
      - [Input](#input)
        * [Data Acquisition Plugins](#data-acquisition-plugins)
        * [Policy Storage Plugins](#policy-storage-plugins)
      - [Output](#output)
        * [Resolver Plugins](#resolver-plugins)
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
- [Environment](#environment)
- [Schema](#schema)
- [Functional API](#functional-api)
  * [Access Core API](#access-core-api-1)
    + [Policy Administration Point](#policy-administration-point-1)
    + [Policy Enforcement Point](#policy-enforcement-point-1)
    + [Policy Information Point](#policy-information-point-1)
    + [Policy Decision Point](#policy-decision-point-1)
  * [Access Secure Network API](#access-secure-network-api-1)

## Frontmatter
[frontmatter]: #frontmatter

```yaml
title: Access
stub: access
document: Engineering Specification
version: 0000
maintainer: Bernardo A. Rodrigues <bernardo.araujo@iota.org>
contributors: [Vladimir Vojnovic <vladimir.vojnovic@nttdata.ro>, Golic, Strahinja <strahinja.golic.bp@nttdata.ro>, Sam Chen <sam.chen@iota.org>, Djordje Golubovic <djordje.golubovic@nttdata.ro>]
sponsor: [Jakub Cech <jakub.cech@iota.org>, Mathew Yarger <mathew.yarger@iota.org>]
licenses: ["Apache-2.0"]
updated: 2020-05-21
```

<!--
Engineering Specifications inform developers about the exact shape of
the way the piece of software was built, using paradigms relevant to the
programming language that this project has been built with. The document
seeks to describe in exacting detail "how it works". It describes a
specific implementation of a logical design.

In some cases there may not be a separate logical specification, so the
Implementation Specification documents the design of a reference
implementation that satisfies the requirements set out in the
Behavioral and Structural Requirements Specifications.
-->

## Summary
[summary]: #summary
<!--
Short summary of this document.
-->

This document contains technical information about **Access Core Server**. It is meant to guide engineers willing to collaborate with the project.

Apart from Access Core Server, the Access framework also includes the [Access Mobile Client](https://github.com/iotaledger/access-mobile), which is used to create the user experience for Policy Creation and Access Requests.

## Logical System Design
[system-design]: #system-design
<!--
Please describe all of the current components of the system from a logical
perspective.
-->

The Figure below demonstrates the conceptual relationship between different Access components.

It can be divided in 4 stacked layers:
- Application Layer
- Actor Layer
- API Layer
- Portability Layer

![drawing](/specs/.images/access_structure.png)

The **Portability Layer** implements platform-dependent code. Anything related to drivers, Operating Systems and base libraries.

The **API Layer** implements platform-agnostic code. It's where Access Core, Access Secure Network, and IOTA functionality are implemented.

The **Actor Layer** is where different functionality gets implemented as [Actors](https://en.wikipedia.org/wiki/Actor_model) abstractions. It's where API calls are put together into cohesive blocks of specific functionality.

The **Application Layer** is where the Supervisor works as the main orchestrator that makes all Actors interact with each other. User Configurations are set in place, threads are initiated, and Actors are set up.

Together, the Portability and API Layers form the **Access Core Software Development Kit**.

Together, the Actor and Application Layers form the **Access Core Server Reference Implementation**.

###  Access Core Software Development Kit (ACSDK)

#### Access Core API
The Access Core API is divided into 4 different modules (and a few submodules):
- Policy Administration Point (PAP)
- Policy Information Point (PIP)
- Policy Enforcement Point (PEP)
- Policy Decision Point (PDP)

![drawing](/specs/.images/pxp.png)

##### Policy Administration Point

**PAP** is used for managing updates to policies and determining which policies apply to what requests. The API expects callback functions to be registered as **Policy Storage Plugins**.

##### Policy Enforcement Point

**PEP** routes the access request to the PDP for decision making, and acts on the received decision as appropriate within the system context. The API expects callback functions to be registered as **Resolver Plugins**.

##### Policy Information Point

**PIP** coordinates the determination of attribute values used by the PDP. Attribute values are information collected from the outside world, such as IOTA transactions, sensor data or network traffic. The API expects callback functions to be registered as **Data Acquisition Plugins**.

##### Policy Decision Point

**PDP** is responsible for calculating the output for access requests. The API only consumed internally by other Core API modules.

On action request, Policy Enforcement Point (PEP) requests decision from Policy Decision Point (PDP), providing a `policyID` from the request. Based on the requested `policyID`, PDP  requests policy from the policy store and, if response is valid policy, it proceeds with calculation of decision. For both `policy goc` and `policy doc`, PDP calculates result of the policy, `true` or `false`, and then combines those results into decision, which can be one of four defined values, grant, deny, conflict or gap, and returns this result to PEP.

![drawing](/specs/.images/pdp.png)

Calculation of `policy goc` and `policy doc` are modules that recursively solve every operation in the policy and return result at the end. The module is executed recursively as long as attributes in the attribute list of the policy contain predefined operations.

![drawing](/specs/.images/pdp2.png)

#### Platform Plugins
Since IOTA Access can be used for different use cases, the underlying hardware used for resolving access decisions or for acquisition of data can take many different shapes and forms.

Platform Plugins are used to allow a platform-agnostic approach for different Access implementations.

Platform Plugins are written as callback functions. They talk to hardware drivers and perform system calls, while respecting the specifications imposed by their correspondent Access Core API modules.

Platform Plugins can be divided into Input and Output categories:

##### Input
###### Data Acquisition Plugins
Data Acquisition Plugins are used by PIP to gather information (Attributes) about the external environment.

For example, a Wallet-based Data Acquisition Plugin is used to verify whether an IOTA transaction was indeed performed. That allows PDP to calculate whether to grant access to the device renter.

Another example is of a GPS-based Data Acquisition Plugin that checks device location and uses this Attribute as input for PDP to make its decisions.

###### Policy Storage Plugins
Policies need to be stored on the device in a non-volatile manner.

Embedded Systems can have different forms of permanent storage. It can be eMMC, SSD, SATA, USB, or many other examples. This variability implies the need for a modular approach for Policy Storage.

Policy Storage Plugins are used by PAP to read and write Policies from non-volatile memory.

##### Output
###### Resolver Plugins
Resolver Plugins are used by PEP to enforce actions in the physical world. They are the main interface for actuators that need to behave in accordance to the Access being granted or denied.

For example, imagine a door that is controlled by a relay attached to the board's GPIO. The board is running the Access Core Server, and PDP has decided to deny the incoming Access Request. That means that the GPIO needs to set the relay such that the door remains locked.

#### Access Secure Network API
The Access Secure Network API is used to authenticate clients, ensure (off-Tangle) channel security and perform message validation.

The ASN authentication protocol is used to secure communication channel of the device. It is based on stripped version of SSH protocol. In order to be used in embedded system protocol must fulfill following requirements:
- Independent of physical communication layer
- Small memory footprint
- No OS dependencies
- Fast execution

The ASN authentication protocol is designed as a module that can be used by both client and server applications. A top level asnAuth API unifies the usage and encapsulates the design, hiding the differences that client and server side facilitate and implement.

Client and server use-cases require different paths in authentication methodology. This leads to diversification of authentication steps for each case.

![drawing](/specs/.images/ASN.png)

To meet embedded requirements two versions of client and server internal module realizations have been defined:

- **libCrypto**: module uses the derivative subset of [OpenSSL]([OpenSSL](https://www.openssl.org/) which is widely used, maintained and verified by the open source community.
- **Tiny embedded** module (based on 3rd party libs) is an even smaller realization of necessary functions required for ASN Authentication.
* HW acceleration is mentioned as an upgrade where applicable (depends of HW)

##### libCrypto module
libCrypto is a module (derivate of OpenSSL) consisting of crypto and hash algorithm realizations needed by ASN Authentication protocol:

- **Diffie-Hellman key exchange**: used for generation and computation of the shared secret.
- **RSA**: used for signing and verification of the shared secrets for authenticity purposes.
- **AES256**: for message encryption and decryption,
- **SHA256**: used during encryption key generation.
- **HMAC-SHA256**: used for data integrity during message exchange.

The module maintains the code and security from the OpenSSL parent library.

##### Tiny Embedded module
Tiny Embedded module uses following cryptographic functions:
- **Curve25519 elliptic curve function**: to compute public key and shared secret for DH exchange.
- **ECDSA secp160r1**: for signing and signature validation.
- **SHA256**: for hashing.
- **AES256**: for message encryption and decryption,
- **HMAC-SHA256**: for computing mac.

Client and server generate 32-byte private key and compute 32-byte public key for DH key exchange using [elliptic Curve25519 function](https://github.com/agl/curve25519-donna). Same function is used to compute 32-byte shared secret after the public keys are exchanged.

In server and client authentication both sides sign shared data with respective 32-byte private keys in order to prove identity and other sides confirms this by validating the signature with 64-byte public key. This is done using [ECDSA secp160r1](https://github.com/kmackay/micro-ecc) elliptic curve for both signing and validation.

After the keys were exchanged server and client both compute 32-byte authentication and encryption keys using [SHA256](https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c) hashing algorithm.

After successful authentication, client and server are able to exchange encrypted message. The algorithm used for encryption and decryption of messages is [AES256](https://github.com/kokke/tiny-AES-c).

Data integrity is achieved by computing mac of the encrypted messages and previously computed integrity keys, with HMAC-SHA256.

##### Server authentication key exchange

After physical connection is established, Access Client generates a [Diffie-Hellman](https://mathworld.wolfram.com/Diffie-HellmanProtocol.html) (DH) private key. Based on the private key, Client computes DH public key. Client sends its DH public key to the Access Core Server running on the Target Device.

Server generates DH key pair using the same algorithm used by client.

Server computes the DH-shared secret `K` and hash `H = hash (client ID || server ID || server public key || client DH public key || server DH public key || shared secret K)`, where Client ID and server ID are identification strings of client and server.

Using its own private key, server also computes `signature = f(private key, H)`.

Server sends server public key, server DH public key, and signature to client.

Client first verifies the server's public key. It computes DH-shared secret `K`, hash `H = hash (client ID || server ID || server public key || client DH public key || server DH public key || shared secret K)` and verifies signature.

![drawing](/specs/.images/key_exchange.png)

##### Client public key authentication protocol

The client public key authentication is executed after server authentication key exchange (described above).

Client calculates hash `H = hash (client ID || server ID || client public key || client DH public key || server DH public key || shared secret K)`, signature `S = f(client private key, hash)`, and sends public key + signature to server.

Server verifies the Client's public key and computes hash `H = hash (client ID || server ID || client public key || client DH public key || server DH public key || shared secret K)`. Then, the server verifies signature `S` on hash.

![drawing](/specs/.images/client_key.png)

##### Data encryption, decryption and validation

When both server and client have shared secret `K` and hash `H`, following encryption keys can be generated:
- Initial IV client to server: `hash(K||H||”A”)`
- Initial IV server to client: `hash(K||H||”B”)`
- Encryption key client to server: `hash(K||H||”C”)`
- Encryption key server to client: `hash(K||H||”D”)`
- Integrity key client to server: `hash(K||H||”E”)`
- Integrity key server to client: `hash(K||H||”F”)`

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
The Access Core Server Reference Implementation is meant to act as a reference for developers who want to write their own applications based on IOTA Access.

It runs on a Raspberry Pi, which means it is easily reproducible by Open Source Software Development Communities.

It showcases how to put together the different pieces from the Access Core SDK into a functional Application.

#### Access Actor
The Access Actor is mainly responsible for consuming the Access Core API. It puts together the different function calls such that Access Requests are properly addressed, and Policies are managed adequately.

#### Wallet Actor
The Wallet Actor is responsible for interacting with the IOTA Tangle. It initiates and checks IOTA Transactions while communicating with an IOTA Node.

The Wallet Actor is also responsible for managing the IOTA seed.

While the initial ACSRI does not support a Secure Element for safe seed storage, this feature is in our roadmap for future integration. Probably, this will be achieved in a modular fashion via Secure Element Plugins for the Wallet Actor.

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

![drawing](/specs/.images/request.png)

1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. Access Actor's PEP Listener and Network Actor's Request Listener Daemon loop.
4. Client and Server exchange keys and a secure connection is established. ASN Request Listener Daemon receives Access Request and forwards to Access Actor's PEP.
5. PEP requests decision from PDP.
6. PAP retrieves Policy from non-volatile memory via Policy Storage Plugin.
7. PIP retrieves Attributes via Data Acquisition Plugin.
8. PDP decies actions + obligations.
9. PEP enforces actions + obligations via Resolver Plugin.
10. Action is logged on the Tangle. Access and Network Actors go back to Listen mode.

### Policy Update Protocol
The Figure below shows a visual representation of the Policy Update Protocol:

![drawing](/specs/.images/update.png)
1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. PAP sends Storage Checksum to Network Actor, who forwards it to the Tangle Policy Store (TPS). If ID sent by PAP matches with the Checksum of Policy List stored on the TPS, then TPS replies ok (nothing to update). If the ID differs, the TPS replies with Policy List (update required).
4. PAP Updater compares the received Policy List with all Policies stored locally. This is done via Policy Storage Plugin.
5. PAP initiates the request for the Policies that differ. Request is relayed to Network Actor, who forwards it to TPS. TPS fetches Policy from IOTA Permanode and replies back.
6. PAP parses and validates the new Policies it receives.
7. PAP stores new Policies via Policy Storage Plugin. PAP goes back to Checksum poll mode.

Note: The protocol described in this section will eventually evolve into a different approach. The entity called `Tangle Policy Store` is a Cloud Server that was inherited by the Legacy Design of early FROST implementations. Eventually this centralized entity will be completely removed and Policy Updates will be done via interactions between Access Actor and Wallet Actor, where the Wallet communicates with an IOTA Permanode.

#### Policy Validation
Policy Validation is the process where Policy structure is checked in order to:
  - Validate JSON.
  - Eliminate reduncancy of a binary circuit.
  - Limit values checks.
  - Check for suspicious conditions.

## Programming Language
[language]: #language
<!--
Please describe the language, minimal version and any other details necessary.
-->

IOTA Access Core Server is designed to run on Embedded Systems and IoT devices. Thus, it is imperative that it is implemented in the C programming language ([C99](http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf)), in order to ensure compatibility with these target devices.

[CMake](https://cmake.org/) v3.11 is used as Build System.

## Environment
[environment]: #environment
<!--
Please describe the environment and any other details necessary.
-->

Access Core Server Reference Implementation is originally designed to run on:
- [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/)
- [Yocto Project](https://www.yoctoproject.org/) and [OpenEmbedded](http://www.openembedded.org/wiki/Main_Page) based Linux Distributions.
- [FreeRTOS](https://www.freertos.org/)

## Schema
[schema]: #schema
<!--
If appropriate, please add the schema here.
-->

Policies are objects with the following data structure:

- **Policy ID**: Hash of the compiled policy object.
- **Policy object**: Object that defines the rules and actions of a policy.
- **Signature object**: The signature algorithm, the resource owner's signature, and the permitted user's public key
- **Hash function**: The hash function that was used to generate the policy ID

<!--
ToDo: verify if this is correct
-->

## Functional API
[api]: #api
<!--
Please use the structural needs of the language used. This may be entirely
generated from code / code comments but must always be kept up to date as
the project grows.

Requirements for functions:
- function name
- parameters with:
  - handle
  - description
  - explicit type / length
  - example
- returns
- errors
-->

### Access Core API

#### Policy Administration Point

<!--
ToDo: add link to Doxygen here
-->
xxx

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

<!--
ToDo: switch this to Doxygen?
-->

```
int asnAuth_init_client(asnSession_t *session, void *ext)
```
Initialization function used by the client application to initialize asnAuth library for client usage.

Return values:
- 0: Ok
- 1: Error

Parameters:
- pointer to `asnSession` structure
- pointer to user-defined memory (callback function data).

```
int asnAuth_init_server(asnSession_t *session, void *ext)
```
Initialization function used by the server application to initialize asnAuth library for server usage.

Return values:
- 0: Ok
- 1: Error

Parameters:
- pointer to `asnSession` structure
- pointer to user-defined memory (callback function data).

```
int asnAuth_set_option(asnSession_t *session, const char *key, unsigned char *value)
```
Function used to setup internal options.
Return values:
- 0: Ok
- 1: Error

<!--
ToDo: finish this
-->
Parameters:
- pointer to `asnSession` structure
- xxx

```
int asnAuth_authenticate(asnSession_t *session)
```
Function performs the algorithm of authentication defined by ASN authentication protocol above.
Return values:
- 0: Ok
- 1: Error

Parameters:
- pointer to `asnSession` structure

```
int asnAuth_send(asnSession_t *session, const unsigned char *data, unsigned short  len)
```
The application calls this function to send a message. All encapsulation, encryption and verification is performed internally.

Return values:
- 0: Ok
- 1: Error

<!--
ToDo: finish this
-->
Parameters:
- pointer to `asnSession` structure
- xxx

```
int asnAuth_receive(asnSession_t *session, unsigned char **data, unsigned short  *len)
```
The application calls this function to receive a message. All encapsulation, decryption and verification is performed internally. Raw data and length is returned.
Return values:
- 0: Ok
- 1: Error

<!--
ToDo: finish this
-->
Parameters:
- pointer to `asnSession` structure
- xxx

```
int asnAuth_release(asnSession_t *session)
```
The function is called when the `asnSession_t` structure is no longer needed. All internal memory is deallocated and the structure cannot be used anymore.
Return values:
- 0: Ok
- 1: Error

Parameters:
- pointer to `asnSession` structure
