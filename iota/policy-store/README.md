# POLICY STORE
## Overview
Policy store consists of interface servers for managing delegation policies on IOTA tangle.
It is able to manage REST and TCP requests and it communicates with IOTA IRI node for storing policies and local SQL database where their tangle addresses are stored.

## 1. Compiling
Store is written in `TypeScript`. To run it, it must be compiled to `JavaScript` using `TypeScript` compiler. Make sure you have `TypeScript` compiler installed on your sistem and run it in root directory:
```bash
tsc
```
It will generate `dist` folder containing `JavaScript` code for store.

## 2. Configuring
Configuring is done via `config`, `.env` and `Dockerfile` files.

### 2.1. Config files
There are two config files, `default.json` used for development, and `production.json` used when running store in production environment. They both have the same structure:
```JSON
{
    "server": {
        "rest": {
            "listeningPort": number
        },
        "tcp": {
            "listeningPort": number
        }
    },
    "db": {
        "host": string,
        "port": number
    },
    "iri": {
        "host": string,
        "port": number
    }
}
```
* `server.rest.listeningPort` - Listening port for REST server.
* `server.tcp.listeningPort` - Listening port for TCP server.
* `db.host` - Host address for database.
* `db.port` - Port number for database.
* `iri.host` - IP address for IRI node.
* `iri.port` - Port number for IRI node.

### 2.2. ENV file
Create `.env` file in root directory. It should contain next values:
```bash
SEED=
POSTGRES_PASSWORD=
POSTRGES_USER=
POSTGRES_DB=
```

* `SEED` - Seed for account to make transactions on IOTA tangle.
* `POSTGRES_PASSWORD` - Password for postgres.
* `POSTRGES_USER` - User for postgres.
* `POSTGRES_DB` - Database to connect to in postgres.

### 2.3. Dockerfile
Published port numbers must match corresponding port numbers in `config` files.

## 3. Start
Install `Docker` on your sistem in order to run store. It is also required to have running and accessible IRI node.

To start store run:
```bash
./start_store.sh
```

## 4. Stop
To stop store run:
```bash
./stop_store.sh
```

## 5. Testing/Developing

For testing and development create private tangle using `one-command-tangle` project which can be found [here](https://github.com/iota-community/one-command-tangle).
