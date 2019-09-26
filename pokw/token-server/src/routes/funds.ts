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
