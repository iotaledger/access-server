import express from 'express';
import { IotaController } from './controllers/iota_controller';
import { CommunicationType } from './controllers/controller';
import bodyParser from 'body-parser';
import * as utils from './utils/utils.js';
import _ from 'lodash';
import logger from './utils/logger';
import policyRouter from './routes/policy_route';
require('dotenv').config();

import config from 'config';

const app = express();

const main = () => {
    let PROVIDER_IP_ADDR: string;
    if (config.has('iri.host')) {
        PROVIDER_IP_ADDR = config.get('iri.host');
    } else {

        logger.error('Must define iri.host inside config file. Exiting...');
        return process.exit(1);
    }

    let PROVIDER_PORT_NUMBER: string;
    if (config.has('iri.port')) {
        PROVIDER_PORT_NUMBER = config.get('iri.port');
    } else {
        logger.error('Must define iri.port inside config file. Exiting...');
        return process.exit(1);
    }

    let SERVER_LISTENING_PORT: string;
    if (config.has('server.rest.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.rest.listeningPort');
    } else {
        logger.error('Must define server.rest.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    const SEED = process.env.SEED;
    if (SEED === undefined) {
        logger.error('Must provide SEED as environment variable. Exiting...');
        return process.exit(1);
    }

    const providerUrl = 'http://' +
        PROVIDER_IP_ADDR +
        ':' +
        PROVIDER_PORT_NUMBER;

    const controller = new IotaController(
        SEED,
        providerUrl,
        CommunicationType.rest
    );

    app.use(bodyParser.json());
    app.use(bodyParser.urlencoded({ extended: false }));

    app.use(policyRouter(controller));

    app.use((error: any, req: any, res: any, next: any) => {
        if (error instanceof SyntaxError)
            res.status(400).send(utils.defaultResponse(true, error.message));
        else
            next();
    });

    app.listen(SERVER_LISTENING_PORT, async () => {
        logger.info(
            'IOTA REST server is running on PORT: ',
            SERVER_LISTENING_PORT
        );
    });
};

main();