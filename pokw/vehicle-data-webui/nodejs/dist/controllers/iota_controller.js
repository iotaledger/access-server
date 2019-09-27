"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const Mam = require('./Mam');
const fs_1 = __importDefault(require("fs"));
const { trytesToAscii } = require('@iota/converter');
const moment_1 = __importDefault(require("moment"));
const lodash_1 = __importDefault(require("lodash"));
const LAST_ROOT_PATH = './data/lastRoot';
class IotaController {
    constructor(ipAddr, port, root, sideKey) {
        const provider = 'http://' + ipAddr.toString() + ':' + port.toString();
        this.mamState = Mam.init(provider);
        this.mamState = Mam.changeMode(this.mamState, 'restricted', sideKey);
        this.sideKey = sideKey;
        this.root = root;
        this.lastMessage = '{}';
        // try to load previous last root from local storage
        try {
            const lastRoot = fs_1.default.readFileSync(LAST_ROOT_PATH, 'utf8').toString();
            this.root = lastRoot;
        }
        catch (err) {
        }
    }
    async getLast() {
        try {
            const data = await Mam.fetch(this.root, 'restricted', this.sideKey);
            if (!lodash_1.default.isUndefined(data)) {
                if (this.root != data.nextRoot) {
                    this.root = data.nextRoot;
                    this.saveRootToFile(data.lastRoot);
                }
                let last = data.messages.slice(-1)[0];
                if (!lodash_1.default.isUndefined(last)) {
                    this.lastMessage = trytesToAscii(last);
                    console.log(moment_1.default().format(), 'Received message:', this.lastMessage);
                }
            }
        }
        catch (err) {
            console.log(moment_1.default().format(), err);
        }
        return Promise.resolve(this.lastMessage);
    }
    saveRootToFile(root) {
        fs_1.default.writeFile(LAST_ROOT_PATH, root, 'utf8', (err) => {
            if (err) {
                return console.log(moment_1.default().format(), err);
            }
        });
    }
}
exports.IotaController = IotaController;
