import { Router } from 'express';
import { defaultResponse } from '../utils/utils';
import Controller from '../controllers/controller';
import logger from '../utils/logger';
import { StringRes } from '../assets/string_res';

let _controller: Controller;

const _router = Router();

_router.put('/account', async function (req, res) {
  let address;

  try {
    address = await _controller.createAccount();
    logger.info('Account successfully created ', address);
  } catch (err) {
    logger.warn('Unable to create account with error ', err);
  }

  if (address === undefined) {
    return res.status(400)
      .send(defaultResponse(true, StringRes.CREATE_ACCOUNT_FAILURE));
  } else {
    return res.status(200)
      .send(defaultResponse(
        false,
        StringRes.CREATE_ACCOUNT_SUCCESS,
        { address }
      ));
  }
});

/**
 * Initializes router with provided controller.
 * 
 * @param controller Controller.
 */
const initialize = (controller: Controller) => {
  _controller = controller;
  return _router
};

export default initialize;
