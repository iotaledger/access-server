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

import { IotaController } from './controllers/iota_controller';
import logger from './utils/logger';
import { mandatoryEnvMissing } from './utils/utils';
require('dotenv').config();

const main = () => {
  try {
    const seed = process.env.SEED;
    if (seed === undefined) {
      logger.error(mandatoryEnvMissing('SEED'));
      return process.exit(1);
    }

    const controller = IotaController.initialize();

    controller.createMasterUserAndFundIt(seed)
      .then((masterAccount) => {
        logger.info('ID of master user:', masterAccount);
      })
      .catch(err => logger.warn(err));
  } catch (err) {
    logger.error(err);
    return process.exit(1);
  }
}

main();
