<h1 align="center">
  <img src="access.png">
</h1>
<h2 align="center">
Access-control framework on the IOTA Distributed Ledger.
</h2>

<p align="center">
  <a href="https://discord.iota.org/" style="text-decoration:none;"><img src="https://img.shields.io/badge/Discord-9cf.svg?logo=discord" alt="Discord"></a>
    <a href="https://iota.stackexchange.com/" style="text-decoration:none;"><img src="https://img.shields.io/badge/StackExchange-9cf.svg?logo=stackexchange" alt="StackExchange"></a>
    <a href="https://github.com/iotaledger/access-server/blob/master/LICENSE" style="text-decoration:none;"><img src="https://img.shields.io/github/license/iotaledger/access-server.svg" alt="Apache 2.0 license"></a>
    <img src="https://github.com/iotaledger/access-server/workflows/C/C++%20CI/badge.svg" alt="C/C++ CI">
</p>

# Access Server Reference Implementation (ASRI)
The Access Server Reference Implementation is meant to act as a reference for developers who want to write their own IOTA Access dApps.

It showcases how to put together the different pieces from the Access SDK into a functional application.

Please note that the release `v0.1.0-alpha` is a [Minimal Viable Product (MVP)](https://www.agilealliance.org/glossary/mvp). It is by no means ready for production.

Please also note that the current state of the `master` branch does not necessarily reflect the state of that release tag. In case you want to inspect the source code of the release, make sure you set the correct release tag.

## Documentation

For documentation, please refer to [Access SDK repository](https://github.com/iotaledger/access-sdk.git).

Please note that the documentation refers to `v0.1.0-alpha`, which does not necessarily reflect the current state of the `master` branch.

# Getting Started
## Clone, Build

ASRI is currently focused on [Raspberry Pi OS](https://www.raspberrypi.org/downloads/).

1 - SSH into the Raspberry Pi.

2 - Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git cmake python3-distutils libfastjson-dev libcurl4-gnutls-dev libtool
```

3 - Clone:
```
$ cd ~
$ git clone --recurse-submodules -j4 --branch v0.1.0-alpha https://github.com/iotaledger/access-server.git
$ cd access-server
```

**Warning ⚠️** 
You are checking out a detached head for the release tag `v0.1.0-alpha`, which will allow you to try out the [Access Mobile Client](https://github.com/iotaledger/access-mobile-client) and the [Access Policy Store](https://github.com/iotaledger/access-policy-store).

If you choose to build from `master` or `develop`, beware that you are about to build a **Work-In-Progress**. You are invited to investigate and help us fix the build and runtime errors you will likely encounter.

4 - Build:
```
$ mkdir build; cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/ext_install
$ make -j4
```

## Configure
Edit `config.ini` to set up runtime parameters.

In case you are testing Access Policy Store, it is important that you set up `policy_store_service_ip` under `[pap]`.

**Warning ⚠️** 

You want to edit `access-server/build/config.ini`, **not** `access-server/config.ini`.

## Run
Simply invoke the `asri` [ELF](https://man7.org/linux/man-pages/man5/elf.5.html).

**Warning ⚠️** If you are using `pep_plugin_relay`, you will need `sudo`. That's because [`pigpio`](http://abyz.me.uk/rpi/pigpio/) uses DMA. `pigpio`'s author warns the following:
> At the moment pigpio on the Pi4B is experimental. I am not sure if the DMA channels being used are safe.

For production scenarios, it is advised that `pigpio` is replaced (by a safe alternative) on any GPIO-based `pep_plugin_*` implementation.

```
$ ./asri
```
