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

The **Context Layer** is where different functionality gets implemented as Context abstractions. It's where API calls are put together into cohesive blocks of specific functionality.

The **Application Layer** is where the Supervisor works as the main orchestrator that makes all Contexts interact with each other. Runtime Configurations are set in place, threads are initiated, and Contexts are set up.

Together, the Portability and API Layers form the **Access Core Software Development Kit**.

Together, the Context and Application Layers form the **Access Server Reference Implementation**.

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

On action request, Policy Enforcement Point (PEP) requests decision from Policy Decision Point (PDP), providing a `policyID` from the request. Based on the requested `policyID`, PDP  requests policy from the policy store and, if response is valid policy, it proceeds with calculation of decision. For both `policy GoC` and `policy DoC`, PDP calculates result of the policy, `true` or `false`, and then combines those results into decision, which can be one of four defined values: `grant`, `deny`, `conflict` or `undef`, and returns this result to PEP.

Note: `GoC` stands for `Grant OR Conflict`, while `DoC` stands for `Denial OR Conflict`. This comes from the logical foundation behind the [Policy Language](/docs/04-policy-specs.md).

![drawing](/docs/images/pdp.png)

Calculation of `policy GoC` and `policy DoC` are modules that recursively solve every operation in the policy and return result at the end. The module is executed recursively as long as attributes in the attribute list of the policy contain predefined operations.

![drawing](/docs/images/pdp2.png)
