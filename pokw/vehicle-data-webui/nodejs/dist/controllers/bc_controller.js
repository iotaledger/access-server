"use strict";
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
    constructor({ providerUrl, ipfsIpAddr, ipfsPortNumber, privateKey }) {
        this.messageFilter = undefined;
        const provider = new Web3.providers.HttpProvider(providerUrl);
        this.web3 = new Web3(provider);
        this.privateKey = privateKey;
        this.ipfs = Ipfs({
            host: ipfsIpAddr,
            port: ipfsPortNumber,
            protocol: 'http'
        });
        try {
            this.previousPath = fs_1.default.readFileSync(PATH_TO_PREVIOUS, 'utf8').toString();
        }
        catch (err) {
        }
    }
    async createMessageFilter() {
        try {
            console.log(moment_1.default().format(), 'Creating message filter');
            const privateKeyId = await this.web3.shh.addPrivateKey(this.privateKey);
            const publicKey = await this.web3.shh.getPublicKey(privateKeyId);
            console.log(moment_1.default().format(), 'Public key:', publicKey);
            const options = {
                privateKeyID: privateKeyId,
                minPow: 0.5,
                ttl: 10
            };
            return this.web3.shh.newMessageFilter(options);
        }
        catch (err) {
            console.log(moment_1.default().format(), 'Error when creating message filter:', err);
            return Promise.reject('Unable to create message filter');
        }
    }
    async getLast(deviceId) {
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
                }
                else {
                    console.log(moment_1.default().format(), 'No "data" in lastFile');
                    return Promise.reject('Unable to get last data');
                }
            }
            else if (this.lastFile != undefined) {
                if ('data' in this.lastFile) {
                    return Promise.resolve(this.lastFile.data);
                }
                else {
                    console.log(moment_1.default().format(), 'No "data" in lastFile');
                    return Promise.reject('Unable to get last data');
                }
            }
            else if (this.previousPath != undefined) {
                const file = await this.ipfsCat(this.previousPath);
                this.lastFile = JSON.parse(file);
                if ('data' in this.lastFile) {
                    return Promise.resolve(this.lastFile.data);
                }
                else {
                    return Promise.reject('Unable to get last data');
                }
            }
            else {
                return Promise.reject('Unable to get last data');
            }
        }
        catch (err) {
            console.log(moment_1.default().format(), 'Error when getting last:', err);
            return Promise.reject('Unable to get last data');
        }
    }
    async getLastNewMessage() {
        try {
            if (this.messageFilter == undefined) {
                try {
                    this.messageFilter = await this.createMessageFilter();
                }
                catch (err) {
                    console.log("error when createMessageFilter", err);
                    throw err;
                }
            }
            try {
                const newMessages = await this.web3.shh.getFilterMessages(this.messageFilter);
                if (newMessages[0] != undefined) {
                    const message = this.web3.utils.toAscii(newMessages[0].payload);
                    console.log(moment_1.default().format(), 'Message received:', message);
                    try {
                        return Promise.resolve(JSON.parse(message));
                    }
                    catch (err) {
                        return Promise.resolve(message);
                    }
                }
                else {
                    return Promise.resolve(undefined);
                }
            }
            catch (err) {
                console.log("error when getFilterMessages", err);
                throw err;
            }
        }
        catch (err) {
            return Promise.reject(err);
        }
    }
    async ipfsCat(path) {
        try {
            const buffer = await this.ipfs.cat(path);
            const file = buffer.toString();
            return Promise.resolve(file);
        }
        catch (err) {
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
