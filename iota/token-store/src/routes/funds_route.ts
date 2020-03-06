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

import { Router } from 'express';
import { defaultResponse } from '../utils/utils';
import logger from '../utils/logger';
import Controller from '../controllers/controller';
import { StringRes } from '../assets/string_res';

let _controller: Controller;

const _router = Router();

_router.put('/fund', async (req, res) => {
  if (!req.body.address) {
    return res.status(400)
      .send(defaultResponse(true, StringRes.MISSING_ADDRESS));
  } else if (!req.body.amount) {
    return res.status(400)
      .send(defaultResponse(true, StringRes.MISSING_AMOUNT));
  } else if (isNaN(req.body.amount)) {
    return res.status(400)
      .send(defaultResponse(true, StringRes.AMOUNT_MUST_BE_NUMBER));
  }

  const address = req.body.address;
  const amount = req.body.amount;

  try {
    await _controller.fund(address, amount);
    logger.info(
      `Successfully funded ${address} `
      + `with amount ${amount}`
    );
    return res.status(200)
      .send(defaultResponse(false, StringRes.FUND_SUCCESS));
  } catch (err) {
    logger.info(
      `Unable to fund ${address} `
      + `with amount ${amount}, `
      + `due to error ${err}`
    );
    return res.status(400)
      .send(defaultResponse(true, StringRes.FUND_FAILURE));
  }
});

_router.put('/send', async (req, res) => {
  let errorMessage: string | undefined;

  if (!req.body.sender)
    errorMessage = StringRes.MISSING_SENDER;
  else if (!req.body.receiver)
    errorMessage = StringRes.MISSING_RECEIVER;
  else if (!req.body.amount)
    errorMessage = StringRes.MISSING_AMOUNT;
  else if (isNaN(req.body.amount))
    errorMessage = StringRes.AMOUNT_MUST_BE_NUMBER;

  if (errorMessage != undefined) {
    const response = defaultResponse(true, errorMessage);
    logger.warn(errorMessage);
    return res.status(400).send(response);
  }

  const sender = req.body.sender;
  const receiver = req.body.receiver;
  const amount = req.body.amount;

  try {
    await _controller.send(sender, receiver, amount);

    logger.info(
      `Successfully sent amount ${amount} `
      + `from ${sender} `
      + `to ${receiver}`
    );

    const response = defaultResponse(false, StringRes.SEND_SUCCESS);
    return res.status(200).send(response);
  } catch (err) {
    logger.warn(
      `Unable to send amount ${amount} `
      + `from ${sender} `
      + `to ${receiver}, `
      + `due to error ${err}`
    );

    const response = defaultResponse(true, StringRes.SEND_FAILURE);
    return res.status(400).send(response);
  }
});

_router.put('/balance', async (req, res) => {
  if (!req.body.address) {
    const response = defaultResponse(true, StringRes.MISSING_ADDRESS);
    logger.warn(StringRes.MISSING_ADDRESS);
    return res.status(400).send(response);
  }

  const address = req.body.address;

  try {
    const balance = await _controller.getBalance(address);
    const message = StringRes.BALANCE_REQUEST_SUCCESS;
    const response = defaultResponse(
      false,
      message,
      { balance }
    );
    logger.info('Response: ', response);
    return res.status(200).send(response);
  } catch (err) {
    logger.warn(
      `Unable to get balance for ${address} `
      + `due to error ${err}`
    );
    const response = defaultResponse(
      true,
      StringRes.BALANCE_REQUEST_FAILURE
    );
    return res.status(400).send(response);
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
