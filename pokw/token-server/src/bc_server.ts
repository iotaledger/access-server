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

import express from 'express';
import bodyParser from 'body-parser';
import userRouter from './routes/user';
import fundRouter from './routes/funds';
import * as BcController from './controllers/bc_controller';
import * as utils from './utils/utils';
import moment from 'moment';
import config from 'config';
require('dotenv').config();

const app = express();

const main = () => {
    let PROVIDER_IP_ADDR: string;
    if (config.has('provider.ipAddress')) {
        PROVIDER_IP_ADDR = config.get('provider.ipAddress');
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

    let SERVER_LISTENING_PORT: string;
    if (config.has('server.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.listeningPort');
    } else {
        console.log('Must define server.listeningPort inside config file. Exiting...');
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
    
    app.listen(SERVER_LISTENING_PORT, async () => {
        console.log(moment().format(), 'Server is running on PORT: ', SERVER_LISTENING_PORT);
    });
    
    const controller = new BcController.Controller(providerUrl, ACCOUNT_PRIV_KEY, CONTRACT_ADDR);
    
    app.use(bodyParser.json());
    app.use(bodyParser.urlencoded({ extended: false }));
    
    app.use(userRouter(controller));
    app.use(fundRouter(controller));
    
    app.use((err: any, req: any, res: any, next: any) => {
        if (err instanceof SyntaxError) {
            return res.status(400).send(utils.defaultResponse(true, 'invalid JSON in body'));
        } else {
            next();
        }
    });
}

main();
