import net from 'net';
import moment from 'moment';
import config from 'config';
import { IotaController } from './controllers/iota_controller';

require('dotenv').config();

const server = net.createServer();
const HOST = '0.0.0.0';

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

    let SERVER_LISTENING_PORT: number;
    if (config.has('server.listeningPort')) {
        SERVER_LISTENING_PORT = config.get('server.listeningPort');
    } else {
        console.log('Must define server.listeningPort inside config file. Exiting...');
        return process.exit(1);
    }

    const SIDE_KEY = process.env.SIDE_KEY;
    if (SIDE_KEY === undefined) {
        console.log('Must provide SIDE_KEY as environment variable. Exiting...');
        return process.exit(1);
    }

    const controller = new IotaController(IRI_IP_ADDR, IRI_PORT, SIDE_KEY);

    server.on('connection', socket => {
        socket.on('data', async data => {
            const receivedData = data.toString();
            try {
                await controller.publish(receivedData, '');
            } catch(err) {
                console.log(moment().format(), err);
                return socket.end(JSON.stringify({
                    error: true,
                    message: 'failed to parse command'
                }));
            }
        });
    });

    server.listen(SERVER_LISTENING_PORT, HOST, () => {
        console.log(moment().format(), `Vehicle data publisher is running on port ${SERVER_LISTENING_PORT}`);
    });
}

main();
