/*
* This file is part of the Frost distribution
* (https://github.com/xainag/frost)
*
* Copyright (c) 2019 XAIN AG.
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
import _ from 'lodash';
import { ControllerInterface, CommunicationType, Body, KnownCommands } from '../controllers/controller_interface';

let _controller: ControllerInterface;

const router = Router();

router.put('/policy', async (req, res) => {
    try {
        const response = await policy(req.body);

        if ('error' in response) {
            return res.status(response.error ? 400 : 200).send(response);
        } else {
            return res.status(200).send(response);
        }
    } catch(err) {
        console.log(err);
        if (_controller.type == CommunicationType.tcp)
            return res.status(400).send({ error: err });
        else 
            return res.status(400).send(utils.defaultResponse(true, err));
    }
});

async function policy(body: Body): Promise<any> {
    if (_.isEmpty(body))
        return Promise.reject('missing body');

    const cmd: string = body.cmd;

    if (_.isEmpty(cmd))
        return Promise.reject('missing command');
    
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
            return Promise.reject('unsupported command'); 
    }
}

/**
 * Initialize router which controller to use
 * 
 * @param controller Controller
 */
const initialize = (controller: ControllerInterface) => {
    _controller = controller;
    return router;
};

export = initialize;
