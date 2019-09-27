"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const Mam = require('@iota/mam');
const fs_1 = __importDefault(require("fs"));
const { asciiToTrytes } = require('@iota/converter');
const moment_1 = __importDefault(require("moment"));
const PATH_TO_STATE = './data/mamState.json';
class IotaController {
    constructor(ipAddr, port, sideKey) {
        const provider = 'http://' + ipAddr.toString() + ':' + port.toString();
        this.mamState = Mam.init(provider);
        this.mamState = Mam.changeMode(this.mamState, 'restricted', sideKey);
        try {
            const savedMamState = JSON.parse(fs_1.default.readFileSync(PATH_TO_STATE, 'utf8'));
            this.mamState.subscribed = savedMamState.subscribed;
            this.mamState.channel = savedMamState.channel;
            this.mamState.seed = savedMamState.seed;
        }
        catch (err) {
        }
    }
    async publish(data, deviceId) {
        // TODO: add deviceId to message
        // Convert the JSON to trytes and create a MAM message
        const trytes = asciiToTrytes(data);
        const message = Mam.create(this.mamState, trytes);
        // Update the MAM state to the state of this latest message
        // We need this so the next publish action knows it's state
        this.mamState = message.state;
        this.saveStateToFile(this.mamState);
        // Attach the message
        try {
            await Mam.attach(message.payload, message.address, 3, 9);
            console.log(moment_1.default().format(), 'Sent to tangle on address:', message.root);
        }
        catch (err) {
            console.log(moment_1.default().format(), err);
        }
    }
    saveStateToFile(state) {
        const jsonContent = JSON.stringify(state, null, 1);
        fs_1.default.writeFile(PATH_TO_STATE, jsonContent, 'utf8', err => {
            if (err) {
                return console.log(moment_1.default().format(), err);
            }
        });
    }
}
exports.IotaController = IotaController;
