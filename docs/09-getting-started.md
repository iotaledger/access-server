# Building Access Server

This tutorial assumes you have already followed:
- [Getting Started with Access Mobile Client]()
- [Getting Started with Access Policy Store]()

Access Server Reference Implementation is currently focused on [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/).

1. SSH into Raspbian.
2. Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git python3-distutils libfastjson-dev libcurl4-gnutls-dev libsqlite3-dev pigpio
```

3. Clone and build IOTA Access:
```
$ cd ~
$ git clone https://github.com/iotaledger/access-server.git
$ cd access
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

