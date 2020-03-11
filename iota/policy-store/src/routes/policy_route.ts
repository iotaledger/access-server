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
import * as utils from '../utils/utils';
import logger from '../utils/logger';
import _ from 'lodash';
import { Controller, CommunicationType, Body, KnownCommands } from '../controllers/controller';
import { StringRes } from '../assets/string_res';

let _controller: Controller;

const router = Router();

router.put('/policy', async (req, res) => {
    try {
        const response = await policy(req.body);
        logger.info('Response: ', response);
        return res.status(response.error ? 400 : 200).send(response);
    } catch (err) {
        let response: any;
        
        if (_controller.type == CommunicationType.tcp)
        response = { error: err };
        else
        response = utils.defaultResponse(true, err);
        
        logger.info('Response: ', response);
        return res.status(400).send(response);
    }
});

async function policy(body: Body): Promise<any> {
    if (_.isEmpty(body))
        return Promise.reject(StringRes.MISSING_BODY);

    const cmd: string = body.cmd;

    if (_.isEmpty(cmd))
        return Promise.reject(StringRes.MISSING_COMMAND);

    switch (cmd) {
        case KnownCommands.addPolicy:
            return _controller.addPolicy(body);

        case KnownCommands.clearPolicyList:
            return _controller.clearPolicyList(body);

        case KnownCommands.getPolicy:
            return _controller.getPolicy(body);

        case KnownCommands.getPolicyList:
            return _controller.getPolicyList(body);

        default:
            return Promise.reject(StringRes.UNSUPPORTED_COMMAND);
    }
}

/**
 * Initialize router which controller to use
 * 
 * @param controller Controller
 */
const initialize = (controller: Controller) => {
    _controller = controller;
    return router;
};

export = initialize;
