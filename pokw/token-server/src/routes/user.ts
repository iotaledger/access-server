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
import { ControllerInterface } from '../controllers/controller_interface';
import moment from 'moment';

let _controller: ControllerInterface;

const router = Router();

router.put('/account', async function(req, res) {
    let address;

    try {
        address = await _controller.createAccount();
        console.log(moment().format(), 'Account successfully created with address:', address);
    } catch (err) {
        console.log(moment().format(), 'Unable to create account with error:', err);
    }

    if (address === undefined) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'unable to create new account')); 
    } else {
        return res.status(200)
            .send(utils.defaultResponse(
                false, 
                'account successfully created',
                {
                    address: address
                }
            ));
    }
});

const initialize = (controller: ControllerInterface) => { 
    _controller = controller;
    return router
};

export = initialize;
