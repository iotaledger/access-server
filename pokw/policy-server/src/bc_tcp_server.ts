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

import net from 'net';
import * as utils from './utils/utils.js';
import * as BcController from './controllers/bc_controller';
import moment from "moment";
import { CommunicationType } from './controllers/controller_interface';
import _ from 'lodash';
import config from 'config';
require('dotenv').config();

const server = net.createServer();

const HOST = '0.0.0.0';

function parseJSON(data: string) {
    let returnValue = null;
    try {
      returnValue = JSON.parse(data);
    } catch (parseError) {
      console.log(moment().format() + " " + parseError);
      console.log(moment().format() + " " + "data raw");
      console.log(moment().format() + " " + data);
      console.log(moment().format() + " " + "data");
      console.log(moment().format() + " " + data.toString());
      returnValue = -1;
    }
    return returnValue;
  }

const main = () => {
    let PROVIDER_IP_ADDR: string;
    if (config.has('provider.ipAddress')) {
        PROVIDER_IP_ADDR= config.get('provider.ipAddress');
    } else {
        console.log('Must define provider.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
   
    let PROVIDER_PORT_NUMBER: string;
    if (config.has('provider.portNumber')) {
        PROVIDER_PORT_NUMBER = config.get('provider.portNumber');
    } else {
        console.log('Must define provider.portNumber inside config file. Exiting...');
        return process.exit(1);
    }

    let SERVER_LISTENING_PORT: number;
    if (config.has('server.tcp.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.tcp.listeningPort');
    } else {
        console.log('Must define server.tcp.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    let CONTRACT_ADDR: string;
    if (config.has('contract.address')) {
        CONTRACT_ADDR = config.get('contract.address');
    } else {
        console.log('Must define contract.address inside config file. Exiting...');
        return process.exit(1);
    }

    const ACCOUNT_PRIV_KEY = process.env.ACCOUNT_PRIV_KEY;
    if (ACCOUNT_PRIV_KEY === undefined) {
        console.log('Must provide ACCOUNT_PRIV_KEY as environment variable. Exiting...');
        return process.exit(1);
    }

    const providerUrl = 'http://' + PROVIDER_IP_ADDR + ':' + PROVIDER_PORT_NUMBER;

    const controller = new BcController.Controller(
        providerUrl,
        ACCOUNT_PRIV_KEY,
        CONTRACT_ADDR,
        CommunicationType.tcp
    );

    server.listen(SERVER_LISTENING_PORT, HOST, () => {
        console.log(`TCP server is running on port ${SERVER_LISTENING_PORT}`);
    });

    server.on('connection', socket => {
        socket.on('data', async data => {
            try {
                const json = parseJSON(data.toString());
                if (json == -1) {
                    return socket.end(JSON.stringify({ error: "Malformed JSON." }));
                }
                const response = await controller.policy(json);
                return socket.end(JSON.stringify(response));
            } catch(err) {
                console.log(err);
                return socket.end(JSON.stringify(utils.defaultResponse(true, err)));
            }
        });
    });
}

main();
