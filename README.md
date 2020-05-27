<h1 align="center">
  <br>IOTA Access<br>
  <img src="access.png">
</h1>
<h2 align="center">
Access-control framework on the IOTA Distributed Ledger.
</h2>

**IOTA Access** is a lightweight and highly flexible access-control framework tailored for resource-constrained settings, such as embedded devices and the infrastructure in which they are used.

The framework is also expanded with relevant concepts, such as obligations and the delegation of access-control policies, to particularly address the needs of reliable and secure human-machine interactions in commercial settings of the IoT and mobility space.

IOTA Access uses access control policies. Attributes are described in combination with binary operations that result in the `[grant, deny, conflict, undefined]` set of logical outcomes for access control of some physical device.

This repository is a Work-in-Progress, and the [Reference Implementation](/docs/02-architecture.md#access-server-reference-implementation-acsri) should be seen as a [*Minimum Viable Product*](https://en.wikipedia.org/wiki/Minimum_viable_product) (MVP).

## Documentation
For newcomers, documentation is the ideal place to start. It will give you an overview of how the project is structured.

The [docs](/docs) directory contains markdown files for documentation:
 - [Introduction](/docs/01-introduction.md)
 - [Architecture](/docs/02-architecture.md)
 - [Getting Started](/docs/03-getting_started.md)

## Specification
[Specification](/specs/HOW-TO-SPEC.md) is the ideal place for those who want to actively interact and contribute with the project. It contains templates for:
 - [Project Scope](/specs/1-SCOPE/access-SCOPE-0000.md)
 - [Request for Proposal](/specs/2-RFP/access-detail-RFP-0000.md)
 - [Engineering Specification](/specs/3-SPECS/access-ENGINEERING-SPEC-0000.md)
 - [Requirements Specification](specs/3-SPECS/access-REQUIREMENTS-SPEC-0000.md)
 - [Request for Comments](/specs/4-RFC/0000-access-RFC.md)
 - [Software Bill of Materials](/specs/5-BOM/access-BOM-0000.md)

 ## Disclaimer

This repository only contains source code for the **Access Server**. The Access framework also includes the [Access Client](https://github.com/iotaledger/access-mobile), which is used to create the user experience for Policy Creation and Access Requests.

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
