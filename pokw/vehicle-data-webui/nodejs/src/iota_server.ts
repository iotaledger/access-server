import express from 'express';
import bodyParser from 'body-parser';
import _ from 'lodash';
import path from 'path';
import moment from 'moment';
import router from './rutes/router';
const cors = require('cors');
require('dotenv').config();
import config from 'config';
import { IotaController } from './controllers/iota_controller';


const app = express();

const main = () => {
    let IRI_IP_ADDR: string;
    if (config.has('iri.ipAddress')) {
        IRI_IP_ADDR= config.get('iri.ipAddress');
    } else {
        console.log('Must define iri.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
   
    let IRI_PORT: string;
    if (config.has('iri.portNumber')) {
        IRI_PORT = config.get('iri.portNumber');
    } else {
        console.log('Must define iri.portNumber inside config file. Exiting...');
        return process.exit(1);
    }

    let SERVER_LISTENING_PORT: string;
    if (config.has('server.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.listeningPort');
    } else {
        console.log('Must define server.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    let ROOT: string;
    if (config.has('mam.root')) {
        ROOT = config.get('mam.root');
    } else {
        console.log('Must define mam.root inside config file. Exiting...');
        return process.exit(1);
    }

    const SIDE_KEY = process.env.SIDE_KEY;
    if (SIDE_KEY === undefined) {
        console.log('Must provide SIDE_KEY as environment variable. Exiting...');
        return process.exit(1);
    }

    app.use(cors({ credentials: true, origin: true}));
    app.use(bodyParser.json());
    app.use(bodyParser.urlencoded({ extended: false }));

    app.listen(SERVER_LISTENING_PORT, async () => {
        console.log(moment().format(), 'HTTP server is running on PORT:', SERVER_LISTENING_PORT);
    });

    const controller = new IotaController(IRI_IP_ADDR, IRI_PORT, ROOT, SIDE_KEY);

    app.use(router(controller));
};

main();
