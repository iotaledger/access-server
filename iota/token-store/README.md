# TOKEN STORE

## Overview

Token store is managing mechanism for user's tokens. It consists of REST server.

## Dependencies
* IOTA Tangle
* IOTA HUB (database as well)

### IOTA Tangle

`Token Store` can directly connect to IOTA main Tangle, but for development and testing purposes you can set up private IOTA Tangle using [one-command-tangle](https://github.com/iota-community/one-command-tangle).

### IOTA HUB

For managing user's tokens on IOTA Tangle in secure manner, official IOTA HUB is used. It consists of local RPC server and database. Instructions for setting it up can be found [here](https://github.com/iotaledger/hub).

## Configuring

Configuring is done using environment variables. You can rename `.env.example` to `.env` file and use it as is. It can define next variables:
* `IRI_HOST` - Host address of IRI node.
* `IRI_PORT` - Port number of IRI node.
* `HUB_HOST` - Host address of IOTA HUB server.
* `HUB_PORT` - Port number of IOTA HUB server.
* `LISTENING_PORT` - Port number on which `Token Store`'s REST server is listening.
* `DECIMALS` - Optional variable. If omitted, default value is 1. It represents number of decimal points used for human readable representation of tokens. For example if DECIMALS=3 and you send someone 27 tokens, internally transaction of 27 000 tokens will be made. This has purpose of simplifying transactions of large number of tokens with low value.
* `MASTER_ID` - ID of master user in IOTA HUB from who's account payments are being made. It can be regularly created account if `Token Store` is connected to IOTA main Tangle, or it can be created using build in tool for initializing IOTA HUB when using private Tangle.
* `SEED` - Seed for master account from who's account payments are being made. You only need to provide it when using private Tangle.

### Initializing when using private Tangle

To initialize IOTA HUB first you must provide `seed` of master account by specifying `SEED` as environment variable. Then run:
```bash
./initialize_hub.sh
```
If everything is successfully executed ID of master account will be yielded. Save it in `.env` file under `MASTER_ID` variable.


## Starting

To start `Token Store` run:
```bash
./start_store.sh
```

## Stopping

To stop `Token Store` run:
```bash
./stop_store.sh
```
