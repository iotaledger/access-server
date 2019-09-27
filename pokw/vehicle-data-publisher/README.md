# VEHICLE DATA PROVIDER
TCP server for publishing data to tangle using MAM protocol.

## 1. Before starting
Before starting the server create `.env` like below
```bash
IRI_IP_ADDR=
IRI_PORT=
SIDE_KEY=
```
For `IRI_IP_ADDR` input ip address of IRI node, eg. `127.0.0.1`.

For `IRI_PORT` input port number of IRI node, default is `14265`.

For generating `SIDE_KEY` you can use simple IOTA SEED generator found [here](https://ipfs.io/ipfs/QmdqTgEdyKVQAVnfT5iV4ULzTbkV4hhkDkMqGBuot8egfA).

## 2. Starting
Start the server by simply running
```bash
./start_server
```

## 3. Obtaining address
To read tangle addresses from log where messages have been published run 
```bash
docker logs vehicle-data-publisher
```


To find next root address, read it from file `data/mamState.json` after data has been published.