module.exports = {
  contracts_build_directory: "./build_sol",
  networks: {
    development: {
     host: "127.0.0.1",
     port: 8545,
     network_id: "*",
     gasPrice: 0
     }
    // private: {
    //   provider: privateProvider,
    //   network_id: "*",
    //   gas: 0
    // }
  },
  mocha: {
    timeout: 10000
  },
  compilers: {
    solc: {
      version: "0.5.0",
       optimizer: {
         enabled: false,
         runs: 200
       }
    }
  }
}
