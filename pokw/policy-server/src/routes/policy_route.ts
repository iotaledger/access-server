/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
