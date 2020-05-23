# Project Scope (Scope)
[Scope]: #Scope

## Frontmatter
[frontmatter]: #frontmatter
```yaml
title: Access
stub: access
document: SCOPE
version: 0000
maintainer: Bernardo A. Rodrigues <bernardo.araujo@iota.org>
contributors: [Vladimir Vojnovic <vladimir.vojnovic@nttdata.ro>, Golic, Strahinja <strahinja.golic.bp@nttdata.ro>, Sam Chen <sam.chen@iota.org>, Djordje Golubovic <djordje.golubovic@nttdata.ro>]
sponsor: Jakub Cech <jakub.cech@iota.org>
licenses: ["Apache-2.0"]
updated: 2020-05-21
```

## License
[license]: #license
<!--
Please specify licenses here and in the frontmatter.
-->
All code is licensed under the [Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0.html)., all text and images are licensed under the [Creative Commons Attribution 4.0 International](https://creativecommons.org/licenses/by/4.0/).

## Language
[language]: #language
<!--
Do not change this section.
-->
The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" in this document are to be interpreted as described in RFC 2119.

## Versioning
[versioning]: #versioning
<!--
Do not change this section.
-->
These documents MUST use incremental numbering. New documents always start at 0000. Subsequent revisions to each RFI, RFP and RFC will have their number increased by one.

Software releases will follow [strict semantic versioning](https://semver.org/).

## Hierarchy
[hierarchy]: #hierarchy
<!--
Do not change this section.
-->
All documents in this specification are understood to flow from this document and be bound to its licenses and language as described above.

## Summary
[summary]: #summary
<!--
One paragraph explanation of the feature.
-->

IOTA Access is a lightweight and highly flexible access-control framework and language tailored for resource-constrained settings, such as embedded devices and the infrastructure in which they are used. The framework is also expanded with relevant concepts, such as obligations and the delegation of access-control policies, to particularly address the needs of reliable and secure human-machine interactions in commercial settings of the IoT and mobility space.

## Motivation
[motivation]: #motivation
<!--
Why are we doing this? What use cases does it support? What is the expected outcome?
-->

Imagine the following scenario: Alice owns a Device that she wants to rent for Bob for x IOTAs. Bob will be able to use this device after transferring x IOTAs to the Device's (or Alice's) Wallet. **IOTA Access** enables this kind of scenario.

## Product Introduction
[product]: #product
<!--
Talk about the business reasons for the product's existence, what it is for and who it serves.
-->

IOTA Access can be adopted in many business scenarios where Access Control is used to allow customers to access physical objects or virtual data under specific conditions.

A few examples:
 - Car Rentals
 - Scooter Rentals
 - Building Access Control

## Stakeholders
[stakeholders]: #stakeholders
<!--
- Who are the stakeholders?
- How has the community been involved?
-->
The list of Stakeholders can be summarized as:
- [IOTA Foundation](https://iota.org).
- IOTA Community.
- [XAIN AG](https://www.xain.io/).

The IOTA Foundation is working on the integration of FROST into the IOTA Ecosytem, which then gives birth to IOTA Access.

The IOTA Community (Corporate Partners, developers) will integrate IOTA Access into their own products.

XAIN AG. was the initial implementer of the product (previously known as [FROST](https://xain-documentation.readthedocs.io/en/latest/Frost/)), so it is in their interest what Access eventually evolves into.

## Guide-level explanation
[guide-level-explanation]: #guide-level-explanation
<!--
Explain the proposal as if it was already included in the language and you were
teaching it to another programmer. That generally means:

- Introducing new named concepts.
- Explaining the feature largely in terms of examples.
- Explaining how programmers should *think* about the feature, and how it should impact the
 way they use this software. It should explain the impact as concretely as possible.
- If applicable, provide sample error messages, deprecation warnings, or migration guidance.
- If applicable, describe the differences between teaching this to existing programmers
and new programmers.
-->

The term Access Control refers to the secure evaluation and protection of access requests from entities or  subjects to resources or objects. The subjects might either be human or machines and the resource can refer to physical objects or virtual data.

The area of Access Control provides models, methods, and tools for controlling which agents – humans, mechanical devices or AIs – have access to what resources and under which circumstances. Many modern access-control architectures formulate the intended access control in a policy. One advantage of policy-based access control is that it supports the composition and mobility of controls – making it ideal for open and distributed systems (such as the IOTA DLT).

Access control is a key security mechanism in enterprise systems, operating systems, physical buildings – to name a few prominent applications. An access-control policy may be somewhat static, e.g., saying which areas in an airport terminal are accessible to passengers who went through a security check already. However, such policies may also be more dynamic and allow for the delegation of access privileges to other agents, e.g. the owner of a vehicle can grant a delivery person access to the trunk of the vehicle under certain conditions – say, within a given time interval. Access to the trunk then creates an obligation, to trigger a notification to the owner that access did indeed occur.

## Prior art
[prior-art]: #prior-art
<!--
Discuss prior art, both the good and the bad, in relation to this proposal.
A few examples of what this can include are:

- For language, library, tooling, and compiler proposals: Does this feature
exist in other similar projects and what experience have their community had?
- For community proposals: Is this done by some other community and what were their
experiences with it?
- For other teams: What lessons can we learn from what other communities have done here?
- Papers: Are there any published papers or great posts that discuss this? If you
have some relevant papers to refer to, this can serve as a more detailed theoretical background.

If there is no prior art, that is fine - your ideas are interesting to us whether
they are brand new or if it is an adaptation from other projects.
-->
Here's a list of resources on XAIN FROST:

* [Conference Paper] **Owner-centric sharing of physical resources, data, and data-driven insights in digital ecosystems**: https://spiral.imperial.ac.uk:8443/handle/10044/1/77522
* [ReadTheDocs] **FROST — Xain Documentation**: https://xain-documentation.readthedocs.io/en/latest/Frost/
* [Video Presentation] **Michael Huth (CTO) presents FROST at Birmingham**: https://www.youtube.com/watch?v=2mHQrmGt7CA
* [Medium Article] **A technical overview of XAIN Embedded Extension**: https://medium.com/xain/a-technical-overview-of-xain-embedded-905678f5b236
* [Medium Article] **Overview of the XAIN Pilot for Porsche Vehicles**: https://medium.com/xain/part-1-technical-overview-of-the-porsche-xain-vehicle-network-f70bb117be16
* [Medium Article] **The Porsche-XAIN Vehicle Blockchain Network: A Technical Overview**: https://medium.com/next-level-german-engineering/the-porsche-xain-vehicle-blockchain-network-a-technical-overview-e1f48c40e73d
* [Marketing Video] **XAIN and Porsche: Results of the pilot project**: https://www.youtube.com/watch?v=KvyF78RTj18
* [Presentation Slides] **Access Control via Belnap Logic: Intuitive, Expressive, and Analyzable Policy Composition**: http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf


## Unresolved questions
[unresolved-questions]: #unresolved-questions

<!--
- What parts of the design do you expect to resolve through the spec process
before this gets merged?
- What parts of the design do you expect to resolve through the implementation
of this product?
- What related issues do you consider out of scope for this prodect that could
be addressed in the future independently of the solution that comes out it?
-->

- Access Request via IOTA Tx.
- Complete removal of Policy Store.
- Usage of Chronicle (permanode) for Policy Updates via IOTA Tx.
- Wallet-based Resolver Plugin.
- Wallet-based Data Acquisition Plugin.

## Future possibilities
[future-possibilities]: #future-possibilities
<!--
Think about what the natural extension and evolution of your proposal would
be and how it would affect the language and project as a whole in a holistic
way. Try to use this section as a tool to more fully consider all possible
interactions with the project in your proposal.

Also consider how the this all fits into the roadmap for the project
and of the relevant sub-team.

If you have tried and cannot think of any future possibilities,
you may simply state that you cannot think of anything.
-->

- Streams-based Resolver Plugin.
- Streams-based Data Acquisition Plugin.
- Integration with Smart Contracts.
