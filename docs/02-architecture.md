**Table of Contents:**

- [Access Core Architecture](#access-core-architecture)
- [Access Core Software Development Kit (ACSDK)](#access-core-software-development-kit-acsdk)
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
- [Access Core Server Reference Implementation (ACSRI)](#access-core-server-reference-implementation-acsri)
  * [Access Actor](#access-actor)
  * [Wallet Actor](#wallet-actor)
  * [Network Actor](#network-actor)
  * [Application Supervisor](#application-supervisor)
- [Access Policy](#access-policy)
- [Access Request Protocol](#access-request-protocol)
- [Policy Update Protocol](#policy-update-protocol)

Note: for an in-depth explanation of some of the components below, please refer to the [Engineering Specifications](/specs/3-SPECS/access-ENGINEERING-SPEC-0000.md).

# Access Core Architecture

The Figure below demonstrates the conceptual relationship between different Access Core components.

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

## Platform Plugins
Since IOTA Access can be used for different use cases, the underlying hardware used for resolving access decisions or for acquisition of data can take many different shapes and forms.

Platform Plugins are used to allow a platform-agnostic approach for different Access implementations.

Platform Plugins are written as callback functions. They talk to hardware drivers and perform system calls, while respecting the specifications imposed by their correspondent Access Core API modules.

Platform Plugins can be divided into Input and Output categories:

- Input
   - Data Acquisition Plugins
   - Policy Storage Plugins
- Output
   - Resolver Plugins

## Access Secure Network API
The Access Secure Network API is used to authenticate clients, ensure (off-Tangle) channel security and perform message validation.

The ASN authentication protocol is used to secure communication channel of the device. It is based on stripped version of SSH protocol. In order to be used in embedded system protocol must fulfill following requirements:
- Independent of physical communication layer
- Small memory footprint
- No OS dependencies
- Fast execution

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
