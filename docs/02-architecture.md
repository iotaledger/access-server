- [Access Architecture](#access-architecture)
- [Access Core Software Development Kit (ACSDK)](#access-core-software-development-kit--acsdk-)
  * [Access Core API](#access-core-api)
  * [Platform Plugins](#platform-plugins)
    + [Input](#input)
      - [Data Acquisition Plugins](#data-acquisition-plugins)
      - [Policy Storage Plugins](#policy-storage-plugins)
    + [Output](#output)
      - [Resolver Plugins](#resolver-plugins)
      - [Data Sharing Plugins](#data-sharing-plugins)
  * [Access Secure Network API](#access-secure-network-api)
    + [EdDSA Ed25519](#eddsa-ed25519)
    + [OpenSSL](#openssl)
- [Access Core Server Reference Implementation (ACSRI)](#access-core-server-reference-implementation--acsri-)
  * [Access Actor](#access-actor)
  * [Wallet Actor](#wallet-actor)
  * [Network Actor](#network-actor)
  * [Application Supervisor](#application-supervisor)
- [Access Policy](#access-policy)
- [Access Request Protocol](#access-request-protocol)
- [Policy Update Protocol](#policy-update-protocol)

# Access Architecture

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

# Access Core Software Development Kit (ACSDK)

The Access Core SDK is composed by:
- Access Core API
- Platform Plugins

## Access Core API
The Access Core API is divided into 4 different modules (and a few submodules):
- Policy Administration Point (PAP)
- Policy Information Point (PIP)
- Policy Enforcement Point (PEP)
- Policy Decision Point (PDP)

![drawing](/specs/.images/pxp.png)

**PDP** is responsible for calculating the output for access requests. The API only consumed internally by other Core API modules.

**PAP** is used for managing updates to policies and determining which policies apply to what requests. The API expects callback functions to be registered as **Policy Storage Plugins**.

**PEP** routes the access request to the PDP for decision making, and acts on the received decision as appropriate within the system context. The API expects callback functions to be registered as **Resolver Plugins**.

**PIP** coordinates the determination of attribute values used by the PDP. Attribute values are information collected from the outside world, such as IOTA transactions, sensor data or network traffic. The API expects callback functions to be registered as **Data Acquisition Plugins**.

## Platform Plugins
Since IOTA Access can be used for different use cases, the underlying hardware used for resolving access decisions or for acquisition of data can take many different shapes and forms.

Platform Plugins are used to allow a platform-agnostic approach for different Access implementations.

Platform Plugins are written as callback functions. They talk to hardware drivers and perform system calls, while respecting the specifications imposed by their correspondent Access Core API modules.

Platform Plugins can be divided into Input and Output categories:

### Input
#### Data Acquisition Plugins
Data Acquisition Plugins are used by PIP to gather information (Attributes) about the external environment.

For example, a Wallet-based Data Acquisition Plugin is used to verify whether an IOTA transaction was indeed performed. That allows PDP to calculate whether to grant access to the device renter.

Another example is of a GPS-based Data Acquisition Plugin that checks device location and uses this Attribute as input for PDP to make its decisions.

#### Policy Storage Plugins
Policies need to be stored on the device in a non-volatile manner.

Embedded Systems can have different forms of permanent storage. It can be eMMC, SSD, SATA, USB, or many other examples. This variability implies the need for a modular approach for Policy Storage.

Policy Storage Plugins are used by PAP to read and write Policies from non-volatile memory.

### Output
#### Resolver Plugins
Resolver Plugins are used by PEP to enforce actions in the physical world. They are the main interface for actuators that need to behave in accordance to the Access being granted or denied.

For example, imagine a door that is controlled by a relay attached to the board's GPIO. The board is running the Access Core Server, and PDP has decided to deny the incoming Access Request. That means that the GPIO needs to set the relay such that the door remains locked.

#### Data Sharing Plugins
<!--
ToDo: write this
-->
xxx

## Access Secure Network API
The Access Secure Network API is used to authenticate clients, ensure (off-Tangle) channel security and perform message validation.

### EdDSA Ed25519
[Digital Signature Algorithms](https://en.wikipedia.org/wiki/Digital_Signature_Algorithm) (DSA) are used to verify user and device identities.

ASN uses [EdDSA](https://cryptobook.nakov.com/digital-signatures/eddsa-and-ed25519) (Edwards-curve Digital Signature Algorithm), a modern and secure digital signature algorithm based on performance-optimized elliptic curves.

The EdDSA variant used in ASN is Ed25519, which is based on the [Curve25519](https://en.wikipedia.org/wiki/Curve25519) Elliptic Curve.
 Ed25519 uses small private keys, small public keys and small signatures with high security level at the same time.

### OpenSSL
[OpenSSL](https://www.openssl.org/) is the Transport Layer Security (TLS) and Secure Sockets Layer (SSL) implementation used to secure communication between Access Core Server and an Access Mobile Client.

It ensures that traffic is properly encrypted, providing both privacy and integrity for Access Requests.

# Access Core Server Reference Implementation (ACSRI)
The Access Core Server Reference Implementation is meant to act as a reference for developers who want to write their own applications based on IOTA Access.

It runs on a Raspberry Pi, which means it is easily reproducible by Open Source Software Development Communities.

It showcases how to put together the different pieces from the Access Core SDK into a functional Application.

## Access Actor
The Access Actor is mainly responsible for consuming the Access Core API. It puts together the different function calls such that Access Requests are properly addressed, and Policies are managed adequately.

## Wallet Actor
The Wallet Actor is responsible for interacting with the IOTA Tangle. It initiates and checks IOTA Transactions while communicating with an IOTA Node.

The Wallet Actor is also responsible for managing the IOTA seed.

While the initial ACSRI does not support a Secure Element for safe seed storage, this feature is in our roadmap for future integration. Probably, this will be achieved in a modular fashion via Secure Element Plugins for the Wallet Actor.

## Network Actor
The Access Secure Network Actor works as a Daemon that listens for incoming Access Requests.

It is mainly responsible for consuming the Access Secure Network API in order to handle Ed25519 signatures, OpenSSL sessions and validating Access Requests.

## Application Supervisor
The Application Supervisor works as the main orchestrator that makes all Actors interact with each other. User Configurations are set in place, threads and daemons are initiated, and Actors are set up.

# Access Policy
Access Policies are used by the device owner to express under which circumstances his devices will be accessed, and by whom.

The Access Policy Language is instance of the language [PBel](http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf) (pronounced “pebble”). A basic form of policy is captured in a rule. There are two simple types of rules from which more complex policies can be formed:

```
grant if cond_A
deny if cond_B
```
where `cond_*` are logical expressions built from attributes, their values and comparisons, as well as logical operators.

For example, we may specify an access-control rule:

```
grant if (object == vehicle) && (subject == vehicle.owner.daughter) &&
         (action == driveVehicle) &&
         (0900 ≤ localTime) && (localTime ≤ 2000)
```

This rule implicitly refers to the request made by the daughter of the owner of the vehicle to drive that car. This rule applies only if the requested resource is that vehicle, the action is to drive that vehicle, and the requester is the daughter of the owner of that vehicle. In those circumstances, access is granted if the local time is between 9am and 8pm. The intuition is that this rule does not apply whenever its condition `cond` evaluates to `false`, including in cases in which the request is not of that type.

# Access Request Protocol
The Figure below shows a visual representation of the Access Request Protocol:

![drawing](/specs/.images/request.png)

1. Supervisor creates Access, Wallet and Network Actors. Network Actor starts Policy Update and Request Listener Daemons.
2. Access Actor registers Platform Plugin callbacks.
3. Access Actor's PEP Listener and Network Actor's Request Listener Daemon loop.
4. Request Listener Daemon receives Request and forwards to Access Actor's PEP.
5. PEP requests decision from PDP.
6. PAP retrieves Policy from non-volatile memory via Policy Storage Plugin.
7. PIP retrieves Attributes via Data Acquisition Plugin.
8. PDP decies actions + obligations.
9. PEP enforces actions + obligations via Resolver Plugin.
10. Action is logged on the Tangle. Access and Network Actors go back to Listen mode.

# Policy Update Protocol
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

## Policy Validation
Policy Validation is the process where Policy structure is checked in order to:
  - Validate JSON.
  - Eliminate reduncancy of a binary circuit.
  - Limit values checks.
  - Check for suspicious conditions.
