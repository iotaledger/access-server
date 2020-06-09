<h1 align="center">
  <br>IOTA Access<br>
  <img src="https://github.com/iotaledger/access/raw/master/access.png">
</h1>
<h2 align="center">
Access-control framework on the <a href="https://www.iota.org/" target="_blank">IOTA Distributed Ledger</a>.
</h2>

**IOTA Access** is a lightweight and highly flexible access-control framework tailored for resource-constrained settings, such as embedded devices and the infrastructure in which they are used.

The framework is also expanded with relevant concepts, such as obligations and the delegation of access-control policies, to particularly address the needs of reliable and secure human-machine interactions in commercial settings of the IoT and mobility space.

IOTA Access uses [JSON](https://www.json.org/json-en.html) to describe access control policies. Attributes are described in combination with binary operations that result in the `[grant, deny, conflict, undefined]` set of logical outcomes for access control of some physical device.

IOTA Access will soon be integrated into IOTA Streams (previously known as MAM). These are the potential areas for integration:

* As Distributed Access Control (DAC) authentication protocol.
* As a secure layer of the delegation protocol.
* As a secure layer of publisher/subscriber data sharing protocol.

## Repository Overview
This repository is a Work-in-Progress. Nevertheless, it shows the possibilities of IOTA Access.

* The `android` directory contains a smartphone client application used to create access delegation policies for actions and data streams. Also it is used to identify the user and indicate ownership access right to particular asset.
* The `embedded` directory contains the codebase necessary to run IOTA Access on a Raspberry Pi as a Proof of Concept in the context of the Automotive Industry.
* The `iota` directory contains the codebase necessary to run interface servers responsible for managing delegation policies and token stores on the IOTA Tangle. It is meant to be executed on a Cloud Server.
* The `policy_validation` directory contains tooling used to validate policies, such as:
  - Validate JSON.
  - Eliminate reduncancy of a binary circuit.
  - Limit values checks.
  - Check for suspicious conditions.

## Build Instructions
Development of the PoC is currently focused on [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/).

1. SSH into Raspbian.
2. Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git python3-distutils libfastjson-dev libcurl4-gnutls-dev libsqlite3-dev libssl-dev pigpio
```

3. Download, build and install [`pigpio`](http://abyz.me.uk/rpi/pigpio/):
```
$ cd ~
$ wget https://github.com/joan2937/pigpio/archive/master.zip
$ unzip master.zip
$ cd pigpio-master
$ make
$ sudo make install
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

## Run Instructions
1. xxx
2. xxx
3. xxx

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
