

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
