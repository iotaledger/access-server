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
const net_1 = __importDefault(require("net"));
const moment_1 = __importDefault(require("moment"));
const config_1 = __importDefault(require("config"));
const bc_controller_1 = require("./controllers/bc_controller");
require('dotenv').config();
const server = net_1.default.createServer();
const HOST = '0.0.0.0';
const main = () => {
    let NODE_IP_ADDR;
    if (config_1.default.has('node.ipAddress')) {
        NODE_IP_ADDR = config_1.default.get('node.ipAddress');
    }
    else {
        console.log('Must define node.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
    let NODE_PORT;
    if (config_1.default.has('node.portNumber')) {
        NODE_PORT = config_1.default.get('node.portNumber');
    }
    else {
        console.log('Must define node.portNumber inside config file. Exiting...');
        return process.exit(1);
    }
    let SERVER_LISTENING_PORT;
    if (config_1.default.has('server.listeningPort')) {
        SERVER_LISTENING_PORT = config_1.default.get('server.listeningPort');
    }
    else {
        console.log('Must define server.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }
    let PUBLIC_KEY;
    if (config_1.default.has('whisper.pubKey')) {
        PUBLIC_KEY = config_1.default.get('whisper.pubKey');
    }
    else {
        console.log('Must define whisper.pubKey inside config file. Exiting...');
        return process.exit(1);
    }
    let IPFS_IP_ADDR;
    if (config_1.default.has('ipfs.ipAddress')) {
        IPFS_IP_ADDR = config_1.default.get('ipfs.ipAddress');
    }
    else {
        console.log('Must define ipfs.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
    let IPFS_PORT;
    if (config_1.default.has('ipfs.portNumber')) {
        IPFS_PORT = config_1.default.get('ipfs.portNumber');
    }
    else {
        console.log('Must define ipfs.portNumber inside config file. Exiting...');
        return process.exit(1);
    }
    const providerUrl = 'http://' + NODE_IP_ADDR + ':' + NODE_PORT;
    const controller = new bc_controller_1.BcController({
        ethProviderUrl: providerUrl,
        ipfsProviderIpAddr: IPFS_IP_ADDR,
        ipfsProviderPortNum: IPFS_PORT,
        pubKey: PUBLIC_KEY
    });
    server.on('connection', socket => {
        socket.on('data', async (data) => {
            const receivedData = data.toString();
            let json;
            let deviceId;
            let message;
            // try to parse request into JSON
            try {
                json = JSON.parse(receivedData);
            }
            catch (err) {
                console.log(moment_1.default().format(), 'Error:', err);
                return socket.end(JSON.stringify({
                    error: true,
                    message: 'invalid JSON'
                }));
            }
            // get 'data' from request
            if ('data' in json) {
                message = json.data;
            }
            else {
                console.log(moment_1.default().format(), 'Missing data');
                return socket.end(JSON.stringify({
                    error: true,
                    message: 'missing data'
                }));
            }
            // get 'deviceId' from request
            if ('deviceId' in json) {
                deviceId = json.deviceId;
            }
            else {
                console.log(moment_1.default().format(), 'Missing deviceId');
                return socket.end(JSON.stringify({
                    error: true,
                    message: 'missing deviceId'
                }));
            }
            // publish data
            try {
                await controller.publish(message, deviceId);
                return socket.end(JSON.stringify({
                    error: false,
                    message: 'successfully published'
                }));
            }
            catch (err) {
                console.log(moment_1.default().format(), err);
                return socket.end(JSON.stringify({
                    error: true,
                    message: 'failed to parse command'
                }));
            }
        });
    });
    server.on('error', err => {
        console.log(moment_1.default().format(), 'Error:', err);
    });
    server.listen(SERVER_LISTENING_PORT, HOST, () => {
        console.log(moment_1.default().format(), `Vehicle data publisher is running on port ${SERVER_LISTENING_PORT}`);
    });
};
main();
