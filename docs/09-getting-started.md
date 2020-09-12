# Building Access Server

This tutorial assumes you have already followed:
- [Getting Started with Access Mobile Client]()
- [Getting Started with Access Policy Store]()

Access Server Reference Implementation is currently focused on [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/).

1. SSH into Raspbian.
2. Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git python3-distutils libfastjson-dev libcurl4-gnutls-dev libsqlite3-dev pigpio libtool
```

3. Clone and build IOTA Access:
```
$ cd ~
$ git clone https://github.com/iotaledger/access.git
$ cd access
$ mkdir build; cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/ext_install -DTEST=ON
$ make -j4
```

# Configuring Access Server
<!--
ToDo: write this
-->
xxx

# Starting Access Server
<!--
ToDo: write this
-->
xxx

# Writing a Policy File
<!--
ToDo: write this
-->
xxx

# Creating an Access Request
<!--
ToDo: write this
-->
xxx
