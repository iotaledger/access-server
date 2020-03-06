/*
 * This file is part of the Frost distribution
 * (https://github.com/iotaledger/frost)
 * 
 * Copyright (c) 2020 IOTA.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import express from 'express';
import bodyParser from 'body-parser';
import userRouter from './routes/user_route';
import fundRouter from './routes/funds_route';
import { IotaController } from './controllers/iota_controller';
import { defaultResponse } from './utils/utils';
import logger from './utils/logger';
import { mandatoryEnvMissing } from './utils/utils';
require('dotenv').config();


const _app = express();

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

    _app.use(bodyParser.json());
    _app.use(bodyParser.urlencoded({ extended: false }));

    _app.use(userRouter(controller));
    _app.use(fundRouter(controller));

    _app.use((err: any, req: any, res: any, next: any) => {
      if (err instanceof SyntaxError) {
        return res.status(400).send(defaultResponse(true, 'invalid JSON in body'));
      } else {
        next();
      }
    });

    _app.listen(listeningPort, async () => {
      logger.info('Server is listening on PORT: ', listeningPort);
    });
  } catch (err) {
    logger.error(err);
    process.exit(1);
  }
}

main();
