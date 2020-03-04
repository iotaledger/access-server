# Overview
This is code and instructions for running servers with same api for Ethereum ERC20 tokens.

## 1.0 Build
To build project run
```bash
npm install
npm run build
```

## 1.1 Configuration
Inside `/config/bc/production.json` file you can configure
* `server.listeningPort` - Port number on which server will listen.
* `provider.ipAddress` - IP address of ethereum node provider
* `provider.portNumber` - Port number of ethereum node provider
* `contract.address` - Address of deployed contract
  
## 1.2 Private key
Set private key for account by creating `.env` file like next
```bash
ACCOUNT_PRIV_KEY={PRIVATE_KEY}
```
and save the value.

## 2. Deploying contract
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

## 3.1 Starting
Start the server by running
```bash
./bc_start_server.sh
```

## 3.2 Stopping
Stop the servers by running
```bash
./bc_stop_server.sh
```
