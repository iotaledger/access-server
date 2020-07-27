# Architecture

The Figure below demonstrates the conceptual relationship between different ACSRI components.

It can be divided in 4 stacked layers:
- Application Layer
- Session Layer
- API Layer
- Portability Layer

![drawing](/docs/images/arch.svg)

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
