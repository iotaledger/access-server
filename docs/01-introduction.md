# IOTA Access

IOTA Access is a lightweight and highly flexible access-control framework and language tailored for resource-constrained settings, such as embedded devices and the infrastructure in which they are used. The framework provides fine-grained policy based access control functionality to particularly address the needs of reliable and secure human-machine interactions in commercial settings of the IoT and mobility space.

IOTA Access encodes the logic for access control into **Policies**. Attributes are described in combination with binary operations that result in the `[grant, deny, conflict, undefined]` set of logical outcomes for access control of some physical device.

IOTA Access can be adopted in many business scenarios where Access Control is used to allow customers to access physical objects or virtual data under specific conditions.

Integration with the IOTA Distributed Ledger enables payments, logging of access events, and data sharing in a permission-less and immutable (thus auditable) way.

# Use Cases

## Scenario 1

Alice owns a Device that she wants to rent to Bob for x IOTAs. Bob will be able to use this device after transferring x IOTAs to the Device's (or Alice's) Wallet. IOTA Access enables this kind of scenario.

Alice uses Access Mobile Client to create the Policy that delegates device access to renters. Alice stores the Policy on an IOTA Permanode.

Bob uses Access Mobile Client to initiate Access Requests, and his requests are stored on the Tangle.

## Scenario 2
Alice is Carol's mother. Carol just turned 18 and she wants to drive Alice's car. Alice writes a Policy where she allows Carol to drive her car only under specific conditions (such as time, insurance, and GPS location).

Carol's ability to access the car is dictated by Access Policy written by her mom. Alice synchronizes her Policy with the vehicle, and Carol makes Access Requests.

Alice's uses Access Mobile Client to create the Policy that delegates device access to renters. Alice stores the Policy on an IOTA Permanode.

Carol's uses Access Mobile Client to initiate Access Requests, and her requests are stored on the Tangle.

## Scenario 3
Alice owns a company that functions in a building with smart locks. Alice writes a Policy where her employees are allowed to enter the building only under specific conditions (such as time, employee ID, clearance level).

Employees' ability to access the building is dictated by the Policy written by Alice.

Alice uses Access Mobile Client to create the Policy that delegates device access to renters.

Employees use Access Mobile Client to initiate Access Requests, and their requests are stored on the Tangle.
