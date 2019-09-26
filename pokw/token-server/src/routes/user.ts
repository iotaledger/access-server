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
