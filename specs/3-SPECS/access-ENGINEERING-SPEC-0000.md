# Engineering Specification
[engineering-spec]: #engineering-spec

## Frontmatter
[frontmatter]: #frontmatter

```yaml
title: Access
stub: access
document: Engineering Specification
version: 0000
maintainer: Bernardo A. Rodrigues <bernardo.araujo@iota.org>
contributors: [Vladimir Vojnovic <vladimir.vojnovic@nttdata.ro>, Golic, Strahinja <strahinja.golic.bp@nttdata.ro>, Sam Chen <sam.chen@iota.org>, Djordje Golubovic <djordje.golubovic@nttdata.ro>]
sponsor: Jakub Cech <jakub.cech@iota.org>
licenses: ["Apache-2.0"]
updated: 2020-05-21
```

<!--
Engineering Specifications inform developers about the exact shape of
the way the piece of software was built, using paradigms relevant to the
programming language that this project has been built with. The document
seeks to describe in exacting detail "how it works". It describes a
specific implementation of a logical design.

In some cases there may not be a separate logical specification, so the
Implementation Specification documents the design of a reference
implementation that satisfies the requirements set out in the
Behavioral and Structural Requirements Specifications.
-->

## Summary
[summary]: #summary
<!--
Short summary of this document.
-->

This document contains technical information about **Access Core Server**. It is meant to guide engineers willing to collaborate with the project.

Apart from Access Core Server, the Access framework also includes the [Access Mobile Client](https://github.com/iotaledger/access-mobile), which is used to create the user experience for Policy Creation and Access Requests.

## Logical System Design
[system-design]: #system-design
<!--
Please describe all of the current components of the system from a logical
perspective.
-->

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

Please check the [Architecture Documentaion](/docs/02-architecture.md) for an in-depth explanation of all the components.

## Programming Language
[language]: #language
<!--
Please describe the language, minimal version and any other details necessary.
-->

<!--
ToDo: specify C version here
-->

IOTA Access Core Server is designed to run on Embedded Systems and IoT devices. Thus, it is imperative that it is implemented in the C programming language, in order to ensure compatibility with these target devices.

[CMake](https://cmake.org/) v3.11 is used as Build System.

## Environment
[environment]: #environment
<!--
Please describe the environment and any other details necessary.
-->

Access Core Server Reference Implementation is originally designed to run on:
- [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/)
- [Yocto Project](https://www.yoctoproject.org/) and [OpenEmbedded](http://www.openembedded.org/wiki/Main_Page) based Linux Distributions.
- [FreeRTOS](https://www.freertos.org/)

## Schema
[schema]: #schema
<!--
If appropriate, please add the schema here.
-->

<!--
ToDo: write down Policy JSON Schema here
-->
xxx

## Functional API
[api]: #api
<!--
Please use the structural needs of the language used. This may be entirely
generated from code / code comments but must always be kept up to date as
the project grows.

Requirements for functions:
- function name
- parameters with:
  - handle
  - description
  - explicit type / length
  - example
- returns
- errors
-->

<!--
ToDo: add link to Doxygen here
-->
xxx
