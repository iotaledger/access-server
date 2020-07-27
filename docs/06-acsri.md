

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

# Access Request Protocol

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
