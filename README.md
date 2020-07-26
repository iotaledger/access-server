<h1 align="center">
  <img src="access.png">
</h1>
<h2 align="center">
Access-control framework on the IOTA Distributed Ledger.
</h2>

<p align="center">
  <a href="https://discord.iota.org/" style="text-decoration:none;"><img src="https://img.shields.io/badge/Discord-9cf.svg?logo=discord" alt="Discord"></a>
    <a href="https://iota.stackexchange.com/" style="text-decoration:none;"><img src="https://img.shields.io/badge/StackExchange-9cf.svg?logo=stackexchange" alt="StackExchange"></a>
    <a href="https://github.com/iotaledger/iota.c/blob/master/LICENSE" style="text-decoration:none;"><img src="https://img.shields.io/github/license/iotaledger/iota.c.svg" alt="Apache 2.0 license"></a>
    <img src="https://github.com/iotaledger/access/workflows/C/C++%20CI/badge.svg" alt="C/C++ CI">
</p>

**IOTA Access** is a lightweight access-control framework tailored for resource-constrained settings, such as embedded devices and the infrastructure in which they are used.

The framework is also expanded with relevant concepts, such as obligations and the delegation of access-control policies, to particularly address the needs of reliable and secure human-machine interactions in commercial settings of the IoT and mobility space.

IOTA Access uses access control policies. Attributes are described in combination with binary operations that result in the `[grant, deny, conflict, undefined]` set of logical outcomes for access control of some physical device.

## Purpose
Existing access and permissions solutions revolve around having a connection to a centralized system. The permissions and privilege structures are stored in a server or cloud and managed by a centralized entity. The device granting access needs to have a maintained connection to check these access permissions, and it typically has to be stationary and is usually a highly restricted process.

In automotive and smart mobility contexts, none of this works reliably. Vehicles do not have a consistent connection all the time. Constantly checking in with a centralized structure requires high use of bandwidth and poses slew of single points of failure along the data pipeline. With the use of IOTA Access, we look to solve this. IOTA Access decentralizes access and permission structures and is working towards allowing them to be embedded into the devices and the individual processors on those devices directly. This is done with a new policy-based management structure that focuses on device and human interactions in a direct peer to peer context. Policies can be embedded into devices and devices they come in contact with. These policies can be directly managed by the device owner and can allow for a slew of improvements in security and usability.

For example, a vehicle with IOTA Access enabled, could connect to a parking entry station with IOTA Access enabled, and that entry station could directly allow the vehicle to enter and park based on the policy language if it has been approved. Or if there is a payment requirement, the wallet integration built into IOTA Access can allow for direct M2M payments between that vehicle and the parking entry station. No human interaction directly required. The station gives access to the vehicle. The vehicle pays for that access in a predetermined fashion, which could be as granular as by the second charge rates. When the vehicle leaves, the payment stops, and the transaction is concluded. This can work for EV charging, tolling, parking, fast food, usage-based road tax, mobility as a service use cases, or even delivery services. A user want’s a package dropped off in the trunk of their car while they are at work instead of at their house, Access could enable it. A user wants to rent out an autonomous vehicle in the future to make money while they work, then Access could enable that. A homeowner wants a smart lock to directly and securely manage access and payment for that access to a home they are renting out as an AirBnB. Access could enable that. All of these direct, frictionless use cases and more rely on access and permissions systems. And most cannot be done securely or reliably with centralized systems. 

## Disclaimer
IOTA Access is a Work-in-Progress, and the project should be seen as a [*Minimum Viable Product*](https://en.wikipedia.org/wiki/Minimum_viable_product) (MVP).

## Repositories
IOTA Access is divided into a few repositories:
 - [github.com/iotaledger/access](https://github.com/iotaledger/access.git) (this repo) contains the **Access Core Server Reference Implementation** (ACSRI). It consists of the embedded software meant to be executed on the target device for which access will be delegated.
 - [github.com/iotaledger/access-sdk](https://github.com/iotaledger/access-sdk.git) contains the **Access Core Software Development Kit** (ACSDK). It consists of the Core SDK components used as building pieces for the ACSRI, and is meant to be used as the starting point to write new IOTA Access applications.
 - [github.com/iotaledger/access-mobile-client](https://github.com/iotaledger/access-mobile-client.git) contains an Android-based **Access Client Reference Implementation**. It is meant to work as the User Interface both for creating policies and initiating access requests. It will eventually be replaced by a cross-platform implementation.
 - [github.com/iotaledger/access-policy-store](https://github.com/iotaledger/access-policy-store) contains the **Access Policy Store**. It consists of interface servers for managing policies. It will eventually be rendered obsolete when a Permanode solution is rolled out.

## Documentation
For newcomers, documentation is the ideal place to start. It will give you an overview of how the project is structured.

The [docs](/docs) directory contains markdown files for documentation:
 - [Introduction](/docs/01-introduction.md)
 - [Engineering Specification](/docs/02-engineering-specs.md)
 - [Policy Language](/docs/03-policy-language.md)
 - [Getting Started](/docs/04-getting-started.md)

## Build Instructions
Development of the PoC is currently focused on [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/).

These instructions are only related to the source code for the **Access Core Server**. The Access framework also includes the [Access Client](https://github.com/iotaledger/access-mobile), which is used to create the user experience for Policy Creation and Access Requests.

1. SSH into Raspbian.
2. Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git python3-distutils libfastjson-dev libcurl4-gnutls-dev libssl-dev
```

4. Clone and build IOTA Access:
```
$ cd ~
$ git clone https://github.com/iotaledger/access.git
$ cd access
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/ext_install
$ make

```

## Contributing  

Pull Requests are welcome.  
This project uses clang-format to format C/C++ code. Before you make any changes please install the format script via running `./git_hooks/pre_commit_install.sh`.  

## XAIN FROST
IOTA Access is based on [XAIN](https://www.xain.io/)'s **FROST** project, which is the byproduct of [Leif-Nissen Lundbeak](https://www.researchgate.net/profile/Leif_Nissen_Lundbaek)'s 2019 PhD Thesis at Imperial College London.

Here's a list of resources on XAIN FROST:

* [Conference Paper] **Owner-centric sharing of physical resources, data, and data-driven insights in digital ecosystems**: https://spiral.imperial.ac.uk:8443/handle/10044/1/77522
* [ReadTheDocs] **FROST — Xain Documentation**: https://xain-documentation.readthedocs.io/en/latest/Frost/
* [Video Presentation] **Michael Huth (CTO) presents FROST at Birmingham**: https://www.youtube.com/watch?v=2mHQrmGt7CA
* [Medium Article] **A technical overview of XAIN Embedded Extension**: https://medium.com/xain/a-technical-overview-of-xain-embedded-905678f5b236
* [Medium Article] **Overview of the XAIN Pilot for Porsche Vehicles**: https://medium.com/xain/part-1-technical-overview-of-the-porsche-xain-vehicle-network-f70bb117be16
* [Medium Article] **The Porsche-XAIN Vehicle Blockchain Network: A Technical Overview**: https://medium.com/next-level-german-engineering/the-porsche-xain-vehicle-blockchain-network-a-technical-overview-e1f48c40e73d
* [Marketing Video] **XAIN and Porsche: Results of the pilot project**: https://www.youtube.com/watch?v=KvyF78RTj18
* [Presentation Slides] **Access Control via Belnap Logic: Intuitive, Expressive, and Analyzable Policy Composition**: http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf
