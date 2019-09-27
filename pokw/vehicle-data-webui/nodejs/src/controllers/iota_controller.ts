const Mam = require('./Mam');
import fs from 'fs';
const { trytesToAscii } = require('@iota/converter');
import moment from 'moment';
import _ from 'lodash';
import { ControllerInterface } from './controller_interface';

const LAST_ROOT_PATH = './data/lastRoot';

export class IotaController implements ControllerInterface {
    private mamState: any;
    private sideKey: string;
    private root: string;
    private lastMessage: string;

    constructor(ipAddr: string, port: string | number, root: string, sideKey: string) {
        const provider = 'http://' + ipAddr.toString() + ':' + port.toString();
        this.mamState = Mam.init(provider);
        this.mamState = Mam.changeMode(this.mamState, 'restricted', sideKey);
        this.sideKey = sideKey;
        this.root = root;
        this.lastMessage = '{}';

        // try to load previous last root from local storage
        try {
            const lastRoot = fs.readFileSync(LAST_ROOT_PATH, 'utf8').toString();
            this.root = lastRoot;
        } catch (err) {
        }
    }

    public async getLast(): Promise<any> {
        try {
            const data = await Mam.fetch(this.root, 'restricted', this.sideKey);
            if (!_.isUndefined(data)) {
                if (this.root != data.nextRoot) {
                    this.root = data.nextRoot;
                    this.saveRootToFile(data.lastRoot);
                }
                let last = data.messages.slice(-1)[0];
                if (!_.isUndefined(last)) {
                    this.lastMessage = trytesToAscii(last);
                    console.log(moment().format(), 'Received message:', this.lastMessage)
                }
            }
        } catch (err) {
            console.log(moment().format(), err);
        }
        return Promise.resolve(this.lastMessage);
    }

    private saveRootToFile(root: string) {
        fs.writeFile(LAST_ROOT_PATH, root, 'utf8', (err: any) => {
            if (err) {
                return console.log(moment().format(), err);
            }
        });
    }
}