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
const express_1 = __importDefault(require("express"));
const body_parser_1 = __importDefault(require("body-parser"));
const moment_1 = __importDefault(require("moment"));
const router_1 = __importDefault(require("./rutes/router"));
const config_1 = __importDefault(require("config"));
const bc_controller_1 = require("./controllers/bc_controller");
const cors = require('cors');
require('dotenv').config();
const main = () => {
    const app = express_1.default();
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
    const PRIVATE_KEY = process.env.PRIVATE_KEY;
    if (PRIVATE_KEY === undefined) {
        console.log('Must provide PRIVATE_KEY as environment variable. Exiting...');
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
    app.use(cors({ credentials: true, origin: true }));
    app.use(body_parser_1.default.json());
    app.use(body_parser_1.default.urlencoded({ extended: false }));
    app.listen(SERVER_LISTENING_PORT, async () => {
        console.log(moment_1.default().format(), 'HTTP server is running on PORT:', SERVER_LISTENING_PORT);
    });
    const providerUrl = 'http://' + NODE_IP_ADDR + ':' + NODE_PORT;
    const controller = new bc_controller_1.BcController({
        providerUrl: providerUrl,
        ipfsIpAddr: IPFS_IP_ADDR,
        ipfsPortNumber: IPFS_PORT,
        privateKey: PRIVATE_KEY
    });
    app.use(router_1.default(controller));
};
main();
