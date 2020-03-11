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
