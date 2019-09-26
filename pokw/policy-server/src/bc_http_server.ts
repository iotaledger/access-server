/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

import express from 'express';
import * as BcController from './controllers/bc_controller';
import bodyParser from 'body-parser';
import * as utils from './utils/utils.js';
import { CommunicationType } from './controllers/bc_controller';
import path from 'path';
import policyRouter from './routes/policy_route';
import config from 'config';
import https from 'https';
import http from 'http';
import fs from 'fs';
import moment from 'moment';
require('dotenv').config();

const httpsOptions = {
    cert: fs.readFileSync(path.join(__dirname, '..', 'ssl', 'ca.crt')),
    key: fs.readFileSync(path.join(__dirname, '..', 'ssl', 'ca.key'))
}

const app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use((error: any, req: any, res: any, next: any) => {
    if (error instanceof SyntaxError)
        res.status(400).send(utils.defaultResponse(true, error.message));
    else
        next();
});

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

    let HTTP_SERVER_LISTENING_PORT: string;
    if (config.has('server.http.listeningPort')) {
        HTTP_SERVER_LISTENING_PORT = config.get('server.http.listeningPort');
    } else {
        console.log('Must define server.http.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    let HTTPS_SERVER_LISTENING_PORT: string | undefined;
    if (config.has('server.https.listeningPort')) {
        HTTPS_SERVER_LISTENING_PORT = config.get('server.https.listeningPort');
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
        CommunicationType.rest
    );

    app.use(policyRouter(controller));
    
    http.createServer(app)
        .listen(HTTP_SERVER_LISTENING_PORT, () => {
            console.log(moment().format(), 'BC HTTP server is running on PORT:', HTTP_SERVER_LISTENING_PORT);
        });

    if (HTTPS_SERVER_LISTENING_PORT != undefined) {
        https.createServer(httpsOptions, app)
            .listen(HTTPS_SERVER_LISTENING_PORT, () => {
                console.log(moment().format(), 'BC HTTPS server is running on PORT:', HTTPS_SERVER_LISTENING_PORT);
            });
    }
}

main();

        




