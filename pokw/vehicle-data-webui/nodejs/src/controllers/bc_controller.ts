/*
* This file is part of the Frost distribution
* (https://github.com/xainag/frost)
*
* Copyright (c) 2019 XAIN AG.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


const Web3 = require('web3');
import { ControllerInterface } from './controller_interface';
import moment from 'moment';
import fs from 'fs';
const Ipfs = require('ipfs-http-client');

const PATH_TO_PREVIOUS = '/home/node/app/data/previous'

export * from './controller_interface';

type MessageFilter = any;

export class BcController implements ControllerInterface {
    private web3: any;
    private privateKey: string;
    private messageFilter?: MessageFilter = undefined;
    private ipfs: any;
    private previousPath?: string;
    private lastFile?: any;

    constructor({ providerUrl, ipfsIpAddr, ipfsPortNumber, privateKey }: { providerUrl: string; ipfsIpAddr: string; ipfsPortNumber: number; privateKey: string; }) {
        const provider = new Web3.providers.HttpProvider(providerUrl);
        this.web3 = new Web3(provider);
        this.privateKey = privateKey;
        
        this.ipfs = Ipfs({
            host: ipfsIpAddr,
            port: ipfsPortNumber,
            protocol: 'http'
        });

        try {
            this.previousPath = fs.readFileSync(PATH_TO_PREVIOUS, 'utf8').toString();
        } catch (err) {
        }

    }

    private async createMessageFilter(): Promise<MessageFilter> {
        try {
            console.log(moment().format(), 'Creating message filter');
            const privateKeyId = await this.web3.shh.addPrivateKey(this.privateKey);
            const publicKey = await this.web3.shh.getPublicKey(privateKeyId);
            
            console.log(moment().format(), 'Public key:', publicKey);

            const options = {
                privateKeyID: privateKeyId,
                minPow: 0.5,
                ttl: 10
            }

            return this.web3.shh.newMessageFilter(options);
        } catch (err) {
            console.log(moment().format(), 'Error when creating message filter:', err);
            return Promise.reject('Unable to create message filter');
        }
    }

    public async getLast(deviceId: string): Promise<any> {
        try {
            const newPath = await this.getLastNewMessage();
            if (newPath != undefined) {
                this.previousPath = newPath;
                this.storePath(newPath);
                const file = await this.ipfsCat(newPath);
                this.lastFile = JSON.parse(file);
                console.log(this.lastFile);
                if ('data' in this.lastFile) {
                    return Promise.resolve(this.lastFile.data);
                } else {
                    console.log(moment().format(), 'No "data" in lastFile');
                    return Promise.reject('Unable to get last data');
                }
            } else if (this.lastFile != undefined) {
                if ('data' in this.lastFile) {
                    return Promise.resolve(this.lastFile.data);
                } else {
                    console.log(moment().format(), 'No "data" in lastFile');
                    return Promise.reject('Unable to get last data');
                }
            } else if (this.previousPath != undefined) {
                const file = await this.ipfsCat(this.previousPath);
                this.lastFile = JSON.parse(file);
                if ('data' in this.lastFile) {
                    return Promise.resolve(this.lastFile.data);
                } else {
                    return Promise.reject('Unable to get last data');
                }
            } else {
                return Promise.reject('Unable to get last data');
            }
        } catch (err) {
            console.log(moment().format(), 'Error when getting last:', err);
            return Promise.reject('Unable to get last data');
        }
    }

    private async getLastNewMessage(): Promise<string | undefined> {
        try {
            if (this.messageFilter == undefined) {
                try {
                    this.messageFilter = await this.createMessageFilter();
                } catch (err) {
                    console.log("error when createMessageFilter", err);
                    throw err;
                }
            }
            try {
                const newMessages = await this.web3.shh.getFilterMessages(this.messageFilter);
                if (newMessages[0] != undefined) {
                    const message = this.web3.utils.toAscii(newMessages[0].payload);
                    console.log(moment().format(), 'Message received:', message);
                    try {
                        return Promise.resolve(JSON.parse(message));
                    } catch (err) {
                        return Promise.resolve(message);
                    }
                } else {
                    return Promise.resolve(undefined);
                }
            } catch (err) {
                console.log("error when getFilterMessages", err);
                throw err;
            }
        } catch (err) {
            return Promise.reject(err);
        }
    }

    private async ipfsCat(path: string): Promise<any> {
        try {
            const buffer = await this.ipfs.cat(path);
            const file = buffer.toString();
            return Promise.resolve(file);
        } catch (err) {
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
