# POLICY SERVER
Bundle of services for policy store for ETHEREUM.

## 1.0 Build
To build TypeScript run
```bash
npm install
npm run build
```

## 1.1 Configuration

Inside `/config/bc/production.json` file you can configure
* `server.http.listeningPort` - Port number on which server will listen.
* `server.tcp.listeningPort` - Port number on which server will listen for TCP.
* `provider.ipAddress` - IP address of ethereum node provider
* `provider.portNumber` - Port number of ethereum node provider
* `contract.address` - Address of deployed contract

## 1.2 Deploying contract
First install `truffle`
```bash
sudo npm install -g truffle
```
then build contracts
```bash
truffle compile
```

To deploy the contract, run
```bash
./bc_deploy_contract.sh
```
Find `Contract deployed to address` inside logs, and copy its value to `contract.address` inside `config/bc/production.json` file.

## 2.1 Starting

Start the server by running
```bash
./bc_start_servers.sh
```

## 2.2 Stopping

Stop the servers by running
```bash
./bc_stop_servers.sh
```
