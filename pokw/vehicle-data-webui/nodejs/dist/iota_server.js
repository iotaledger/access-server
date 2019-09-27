"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const body_parser_1 = __importDefault(require("body-parser"));
const moment_1 = __importDefault(require("moment"));
const router_1 = __importDefault(require("./rutes/router"));
const cors = require('cors');
require('dotenv').config();
const config_1 = __importDefault(require("config"));
const iota_controller_1 = require("./controllers/iota_controller");
const app = express_1.default();
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
    let ROOT;
    if (config_1.default.has('mam.root')) {
        ROOT = config_1.default.get('mam.root');
    }
    else {
        console.log('Must define mam.root inside config file. Exiting...');
        return process.exit(1);
    }
    const SIDE_KEY = process.env.SIDE_KEY;
    if (SIDE_KEY === undefined) {
        console.log('Must provide SIDE_KEY as environment variable. Exiting...');
        return process.exit(1);
    }
    app.use(cors({ credentials: true, origin: true }));
    app.use(body_parser_1.default.json());
    app.use(body_parser_1.default.urlencoded({ extended: false }));
    app.listen(SERVER_LISTENING_PORT, async () => {
        console.log(moment_1.default().format(), 'HTTP server is running on PORT:', SERVER_LISTENING_PORT);
    });
    const controller = new iota_controller_1.IotaController(IRI_IP_ADDR, IRI_PORT, ROOT, SIDE_KEY);
    app.use(router_1.default(controller));
};
main();
