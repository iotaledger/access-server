const Web3 = require('web3');

const web3 = new Web3('http://127.0.0.1:8885');

const main = async () => {
    const kId = await web3.shh.newKeyPair();
    const pubKey = await web3.shh.getPublicKey(kId);
    const privateKey = await web3.shh.getPrivateKey(kId);
    console.log('kId:       ', kId);
    console.log('pubKey:    ', pubKey);
    console.log('privateKey:', privateKey);
}

main();