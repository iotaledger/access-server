import express from 'express';
import bodyParser from 'body-parser';
import userRouter from './routes/user_route';
import fundRouter from './routes/funds_route';
import { IotaController } from './controllers/iota_controller';
import { defaultResponse } from './utils/utils';
import logger from './utils/logger';
import { mandatoryEnvMissing } from './utils/utils';
require('dotenv').config();


const app = express();

const main = () => {
    const masterId = process.env.MASTER_ID;
    if (masterId === undefined) {
        logger.error(mandatoryEnvMissing('MASTER_ID'));
        return process.exit(1);
    }

    const listeningPort = process.env.LISTENING_PORT;
    if (listeningPort === undefined) {
        logger.error(mandatoryEnvMissing('LISTENING_PORT'));
        return process.exit(1);
    }

    try {
        const controller = IotaController.initialize();

        app.listen(listeningPort, async () => {
            logger.info('Server is listening on PORT: ', listeningPort);
        });
    
        app.use(bodyParser.json());
        app.use(bodyParser.urlencoded({ extended: false }));
    
        app.use(userRouter(controller));
        app.use(fundRouter(controller));
    
        app.use((err: any, req: any, res: any, next: any) => {
            if (err instanceof SyntaxError) {
                return res.status(400).send(defaultResponse(true, 'invalid JSON in body'));
            } else {
                next();
            }
        });
    } catch (err) {
        logger.error(err);
        process.exit(1);
    }
}

main();