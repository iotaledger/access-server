"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const net_1 = __importDefault(require("net"));
const moment_1 = __importDefault(require("moment"));
const config_1 = __importDefault(require("config"));
const iota_controller_1 = require("./controllers/iota_controller");
require('dotenv').config();
const server = net_1.default.createServer();
const HOST = '0.0.0.0';
const main = () => {
    let IRI_IP_ADDR;
    if (config_1.default.has('iri.ipAddress')) {
        IRI_IP_ADDR = config_1.default.get('iri.ipAddress');
    }
    else {
        console.log('Must define iri.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
    let IRI_PORT;
    if (config_1.default.has('iri.portNumber')) {
        IRI_PORT = config_1.default.get('iri.portNumber');
    }
    else {
        console.log('Must define iri.portNumber inside config file. Exiting...');
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
    const SIDE_KEY = process.env.SIDE_KEY;
    if (SIDE_KEY === undefined) {
        console.log('Must provide SIDE_KEY as environment variable. Exiting...');
        return process.exit(1);
    }
    const controller = new iota_controller_1.IotaController(IRI_IP_ADDR, IRI_PORT, SIDE_KEY);
    server.on('connection', socket => {
        socket.on('data', async (data) => {
            const receivedData = data.toString();
            try {
                await controller.publish(receivedData, '');
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
    server.listen(SERVER_LISTENING_PORT, HOST, () => {
        console.log(moment_1.default().format(), `Vehicle data publisher is running on port ${SERVER_LISTENING_PORT}`);
    });
};
main();
