const net = require('net');
const MamSender = require('./mamSender');
const moment = require('moment');

if (process.env.NODE_ENV != 'production') {
    require('dotenv').config();
}


const IRI_IP_ADDR = process.env.IRI_IP_ADDR;
const IRI_PORT = process.env.IRI_PORT;
const SIDE_KEY = process.env.SIDE_KEY;

if (IRI_IP_ADDR == undefined) {
    console.log('Must provide IRI_IP_ADDR. Exiting...');
    return;
}

if (IRI_PORT == undefined) {
    console.log('Must provide IRI_PORT. Exiting...');
    return;
}

if (SIDE_KEY == undefined) {
    console.log('Must provide SIDE_KEY. Exiting...');
    return;
}

const server = net.createServer();
const mamSender = new MamSender(IRI_IP_ADDR, IRI_PORT, SIDE_KEY);

const PORT = 12345;
const HOST = '0.0.0.0';

server.listen(PORT, HOST, () => {
    console.log(moment().format(), `Vehicle data publisher is running on port ${PORT}`);
});

server.on('connection', socket => {
    socket.on('data', async data => {
        const receivedData = data.toString();
        try {
            await mamSender.publish(receivedData);
        } catch(err) {
            console.log(moment().format(), err);
            return socket.end(JSON.stringify({
                error: true,
                message: 'failed to parse command'
            }));
        }
    });
});