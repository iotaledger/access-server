# VEHICLE DATA WEB UI
HTTP server for presenging XAIN vehicle data obtainig

## 1. Before starting
Before starting the server create `nodejs/.env` like below
```bash
IRI_IP_ADDR=
IRI_PORT=
SIDE_KEY=
ROOT=
```
For `IRI_IP_ADDR` input ip address of IRI node, eg. `127.0.0.1`.

For `IRI_PORT` input port number of IRI node, default should be `14265`.

For `SIDE_KEY` input key used by publisher to decript messages on tangle.

For `ROOT` input address of any message published on tangle by _vehicle-data-publisher_.

## 2. Starting
Start the server by simply running
```bash
./start_server
```

## 3. Stopping
Stop the server by simply running
```bash
./stop_server
```
