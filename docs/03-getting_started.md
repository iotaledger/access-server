- [Building Access Core Server](#building-access-core-server)
- [Configuring Access Core Server](#configuring-access-core-server)
- [Starting Access Core Server](#starting-access-core-server)
- [Writing a Policy File](#writing-a-policy-file)
- [Creating an Access Request](#creating-an-access-request)

# Building Access Core Server

Reference Implementation is currently focused on [Raspbian Buster](https://www.raspberrypi.org/blog/buster-the-new-version-of-raspbian/).

1. SSH into Raspbian.
2. Install dependencies:
```
$ sudo apt-get update
$ sudo apt-get install git python3-distutils libfastjson-dev libcurl4-gnutls-dev libsqlite3-dev libssl-dev pigpio
```

3. Clone and build IOTA Access:
```
$ cd ~
$ git clone https://github.com/iotaledger/access.git
$ cd access
$ mkdir build; cd build
$ cmake -DCMAKE_INSTALL_PREFIX=$PWD ..
$ make -j8
```

# Configuring Access Core Server
<!--
ToDo: write this
-->
xxx

# Starting Access Core Server
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
