import net from 'net';
import * as utils from './utils/utils.js';
import * as IotaController from './controllers/iota_controller';
import { CommunicationType, KnownCommands, Body, Controller } from './controllers/controller';
import path from 'path';
import logger from './utils/logger.js';
import _ from 'lodash';
require('dotenv').config();

// set path to config file
process.env['NODE_CONFIG_DIR'] = path.join(__dirname, '..', 'config', 'iota');
import config from 'config';
import { StringRes } from './assets/string_res.js';

const server = net.createServer();

const HOST = '0.0.0.0';

let controller: Controller;

function parseJSON(data: string) {
    let returnValue = null;
    try {
        returnValue = JSON.parse(data);
    } catch (parseError) {
        returnValue = -1;
    }
    return returnValue;
}

const main = () => {
    let PROVIDER_IP_ADDR: string;
    if (config.has('iri.host')) {
        PROVIDER_IP_ADDR= config.get('provider.host');
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

    let SERVER_LISTENING_PORT: number;
    if (config.has('server.tcp.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.tcp.listeningPort');
    } else {
        logger.error('Must define server.tcp.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    const SEED = process.env.SEED;
    if (SEED === undefined) {
        logger.error('Must provide SEED as environment variable. Exiting...');
        return process.exit(1);
    }

    const providerUrl = 'http://' + PROVIDER_IP_ADDR + ':' + PROVIDER_PORT_NUMBER;

    controller = new IotaController.IotaController(
        SEED,
        providerUrl,
        CommunicationType.tcp
    );

    server.listen(SERVER_LISTENING_PORT, HOST, () => {
        logger.info(`TCP server is running on port ${SERVER_LISTENING_PORT}`);
    });

    server.on('connection', socket => {
        socket.on('data', async data => {
            try {
                const json = parseJSON(data.toString());
                logger.info('Request:', json);
                if (json == -1) {
                    return socket.end(JSON.stringify({ error: "Malformed JSON." }));
                }
                const response = await policy(json);
                logger.info('Response:', response);
                return socket.end(JSON.stringify(response));
            } catch(err) {
                logger.error('Error:', err, 'For request:', data.toString());
                return socket.end(JSON.stringify(utils.defaultResponse(true, err)));
            }
        });
    });
};

async function policy(body: Body): Promise<any> {
    if (_.isEmpty(body))
        return Promise.reject(StringRes.MISSING_BODY);

    const cmd: string = body.cmd;

    if (_.isEmpty(cmd))
        return Promise.reject(StringRes.MISSING_COMMAND);
    
    switch (cmd) {
        case KnownCommands.addPolicy:
            return controller.addPolicy(body);

        case KnownCommands.clearPolicyList:
            return controller.clearPolicyList(body);
        
        case KnownCommands.getPolicy:
            return controller.getPolicy(body);

        case KnownCommands.getPolicyList:
            return controller.getPolicyList(body);
            
        default:
            return Promise.reject(StringRes.UNSUPPORTED_COMMAND); 
    }
}

main();