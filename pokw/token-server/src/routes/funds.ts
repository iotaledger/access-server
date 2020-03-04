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
import { ControllerInterface } from '../controllers/controller_interface';
import moment from 'moment';

let _controller: ControllerInterface;

const router = Router();

router.put('/fund', async (req, res) => {
    if (!req.body.address) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'address is required'));
    } else if (!req.body.amount) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'amount is required'));
    } else if (isNaN(req.body.amount)) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'amount must be number'));
    }

    const address = req.body.address;
    const amount = req.body.amount;

    try {
        await _controller.fund(address, amount);
        console.log(moment().format(), `Successfully funded address: ${address} with amount: ${amount}`);
        return res.status(200).send(utils.defaultResponse(false, 'successfully funded'));
    } catch (err) {
        console.log(moment().format(), `Unable to fund address: ${address} with amount: ${amount}, due to error: ${err}`);
        return res.status(400).send(utils.defaultResponse(true, 'unable to fund'));
    }
});

router.put('/send', async (req, res) => {
    if (!req.body.sender) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'sender is required'));
    } else if (!req.body.receiver) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'receiver is required'));
    } else if (!req.body.amount) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'amount is required'));
    } else if (isNaN(req.body.amount)) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'amount must be number'));
    }

    const sender = req.body.sender;
    const receiver = req.body.receiver;
    const amount = req.body.amount;

    try {
        await _controller.send(sender, receiver, amount);
        console.log(moment().format(), `Successfully sent amount: ${amount} from: ${sender} to: ${receiver}`);
        return res.status(200).send(utils.defaultResponse(false, 'successfully sent'));
    } catch (err) {
        console.log(moment().format(), `Unable to send amount: ${amount} from: ${sender} to: ${receiver}, due to error: ${err}`);
        return res.status(400).send(utils.defaultResponse(true, 'unable to send'));
    }
});

router.put('/balance', async (req, res) => {
    if (!req.body.address) {
        return res.status(400)
            .send(utils.defaultResponse(true, 'address is required'));
    }

    const address = req.body.address;

    try {
        const balance = await _controller.getBalance(address);
        console.log(moment().format(), `Balance of: ${address} is: ${balance}`);
        return res.status(200).send(utils.defaultResponse(false, 'balance request', { balance: balance }));
    } catch (err) {
        console.log(moment().format(), `Unable to get balance of: ${address}`);
        return res.status(400).send(utils.defaultResponse(true, 'unable to get balance'));
    }
});

const initialize = (controller: ControllerInterface) => { 
    _controller = controller;
    return router
};

export = initialize;
