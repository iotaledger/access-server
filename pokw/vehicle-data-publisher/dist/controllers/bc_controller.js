"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const Web3 = require('web3');
const moment_1 = __importDefault(require("moment"));
const fs_1 = __importDefault(require("fs"));
const Ipfs = require('ipfs-http-client');
const PATH_TO_PREVIOUS = '/home/node/app/data/previous';
class BcController {
    constructor({ ethProviderUrl, ipfsProviderIpAddr, ipfsProviderPortNum, pubKey }) {
        const provider = new Web3.providers.HttpProvider(ethProviderUrl);
        this.web3 = new Web3(provider);
        this.pubKey = pubKey;
        this.ipfs = Ipfs({
            host: ipfsProviderIpAddr,
            port: ipfsProviderPortNum,
            protocol: 'http'
        });
        try {
            this.previousPath = fs_1.default.readFileSync(PATH_TO_PREVIOUS, 'utf8').toString();
        }
        catch (err) {
        }
        console.log('Previous hash:', this.previousPath);
        console.log('Public key:', pubKey);
    }
    async publish(data, deviceId) {
        const dataToPublishOnIpfs = {
            previous: this.previousPath,
            deviceId: deviceId,
            data: data
        };
        const hash = await this.publishToIpfs(dataToPublishOnIpfs);
        this.previousPath = hash;
        this.storePath(hash);
        return await this.publishToEth(hash);
    }
    /**
     * Publishes data to IPFS.
     *
     * @param data Data to be published to IPFS.
     * @returns Path to published data.
     */
    async publishToIpfs(data) {
        const result = await this.ipfs.add(Buffer.from(JSON.stringify(data)));
        if ('path' in result[0]) {
            return Promise.resolve(result[0].path);
        }
        return Promise.reject('Unable to publish data to IPFS');
    }
    async publishToEth(data) {
        try {
            const result = await this.web3.shh.post({
                pubKey: this.pubKey,
                ttl: 10,
                payload: this.web3.utils.toHex(data),
                powTime: 3,
                powTarget: 0.5
            });
            console.log(moment_1.default().format(), '\n\tmessage:', data, '\n\tsent:', result);
            return Promise.resolve();
        }
        catch (err) {
            console.log(err);
            return Promise.reject(err);
        }
    }
    async storePath(path) {
        try {
            fs_1.default.writeFileSync(PATH_TO_PREVIOUS, path, 'utf-8');
        }
        catch (err) {
            console.log(moment_1.default().format(), 'Error when storing path:', err);
        }
    }
}
exports.BcController = BcController;
