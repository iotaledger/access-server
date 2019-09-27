const Web3 = require('web3');
import { ControllerInterface } from './controller_interface';
import moment from 'moment';
import fs from 'fs';
const Ipfs = require('ipfs-http-client');

const PATH_TO_PREVIOUS = '/home/node/app/data/previous'

export * from './controller_interface';

export class BcController implements ControllerInterface {
    private web3: any;
    private pubKey: string;
    private ipfs: any;
    private previousPath?: string;

    constructor({ ethProviderUrl, ipfsProviderIpAddr, ipfsProviderPortNum, pubKey }: { ethProviderUrl: string; ipfsProviderIpAddr: string; ipfsProviderPortNum: string | number; pubKey: string; }) {
        const provider = new Web3.providers.HttpProvider(ethProviderUrl);
        this.web3 = new Web3(provider);
        this.pubKey = pubKey;
        this.ipfs = Ipfs({
            host: ipfsProviderIpAddr,
            port: ipfsProviderPortNum,
            protocol: 'http'
        });
        try {
            this.previousPath = fs.readFileSync(PATH_TO_PREVIOUS, 'utf8').toString();
        } catch (err) {
        }

        console.log('Previous hash:', this.previousPath);
        console.log('Public key:', pubKey);
    }
    
    public async publish(data: string, deviceId: string): Promise<void> {
        const dataToPublishOnIpfs = {
            previous: this.previousPath,
            deviceId: deviceId,
            data: data
        }

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
    private async publishToIpfs(data: any): Promise<string> {
        const result = await this.ipfs.add(Buffer.from(JSON.stringify(data)));
        if ('path' in result[0]) {
            return Promise.resolve(result[0].path)
        }
        return Promise.reject('Unable to publish data to IPFS');
    }

    private async publishToEth(data: string) {
        try {
            const result = await this.web3.shh.post({
                pubKey: this.pubKey,
                ttl: 10,
                payload: this.web3.utils.toHex(data),
                powTime: 3,
                powTarget: 0.5
            });
            console.log(moment().format(), '\n\tmessage:', data, '\n\tsent:', result);
            return Promise.resolve();
        } catch (err) {
            console.log(err);
            return Promise.reject(err);
        }
    }

    private async storePath(path: string) {
        try {
            fs.writeFileSync(PATH_TO_PREVIOUS, path, 'utf-8');
        } catch (err) {
            console.log(moment().format(), 'Error when storing path:', err);
        }
    }
}