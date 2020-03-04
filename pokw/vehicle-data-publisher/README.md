# VEHICLE DATA PROVIDER
TCP server for publishing data to tangle using MAM protocol.

## 1. Before starting
Before starting the server run
```bash
npm install
```

After that build project by running
```bash
npm run build
```

## 2. Starting
Start the server by simply running
```bash
./bc_start
```

## 3. Obtaining address
To read tangle addresses from log where messages have been published run 
```bash
docker logs vehicle-data-publisher
```
