const Mam = require('@iota/mam');
import fs from 'fs';
const { asciiToTrytes } = require('@iota/converter');
import moment from 'moment';
import { ControllerInterface } from './controller_interface';

const PATH_TO_STATE = './data/mamState.json';

export class IotaController implements ControllerInterface {
    private mamState: any;

    constructor(ipAddr: string, port: number | string, sideKey: string) {
        const provider = 'http://' + ipAddr.toString() + ':' + port.toString();
        this.mamState = Mam.init(provider);
        this.mamState = Mam.changeMode(this.mamState, 'restricted', sideKey);
        try {
            const savedMamState = JSON.parse(fs.readFileSync(PATH_TO_STATE, 'utf8'));
            this.mamState.subscribed = savedMamState.subscribed;
            this.mamState.channel = savedMamState.channel;
            this.mamState.seed = savedMamState.seed;
        } catch (err) {
        }
    }

    public async publish(data: string, deviceId: string): Promise<void> {
        // TODO: add deviceId to message
        // Convert the JSON to trytes and create a MAM message
        const trytes = asciiToTrytes(data)
        const message = Mam.create(this.mamState, trytes)
        // Update the MAM state to the state of this latest message
        // We need this so the next publish action knows it's state
        this.mamState = message.state
        this.saveStateToFile(this.mamState)
    
        // Attach the message
        try {
            await Mam.attach(message.payload, message.address, 3, 9);
            console.log(moment().format(), 'Sent to tangle on address:', message.root);
        } catch (err) {
            console.log(moment().format(), err);
        }
    }

    private saveStateToFile(state: string) {
        const jsonContent = JSON.stringify(state, null, 1);
        fs.writeFile(PATH_TO_STATE, jsonContent, 'utf8', err => {
            if (err) {
                return console.log(moment().format(), err);
            }
        });
    }
}

