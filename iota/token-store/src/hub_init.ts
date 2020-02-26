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
