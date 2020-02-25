import { Router } from 'express';
import { defaultResponse } from '../utils/utils';
import logger from '../utils/logger';
import { Controller } from '../controllers/controller';
import { StringRes } from '../assets/string_res';
import { info } from 'ethers/errors';

let _controller: Controller;

const router = Router();

router.put('/fund', async (req, res) => {
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
        logger.info(`Successfully funded address: ${address} with amount: ${amount}`);
        return res.status(200).send(defaultResponse(false, 'successfully funded'));
    } catch (err) {
        logger.info(`Unable to fund address: ${address} with amount: ${amount}, due to error: ${err}`);
        return res.status(400).send(defaultResponse(true, 'unable to fund'));
    }
});

router.put('/send', async (req, res) => {
    let message: string | undefined;

    if (!req.body.sender)
        message = StringRes.MISSING_SENDER;
    else if (!req.body.receiver)
        message = StringRes.MISSING_RECEIVER;
    else if (!req.body.amount)
        message = StringRes.MISSING_AMOUNT;
    else if (isNaN(req.body.amount))
        message = StringRes.AMOUNT_MUST_BE_NUMBER;


    if (message != undefined) {
        const response = defaultResponse(true, message);
        logger.info('Response: ', response);
        return res.status(400).send(response);
    }

    const sender = req.body.sender;
    const receiver = req.body.receiver;
    const amount = req.body.amount;

    try {
        await _controller.send(sender, receiver, amount);
        const message = `Successfully sent amount: ${amount} from: ${sender} to: ${receiver}`;
        const response = defaultResponse(false, message);
        logger.info('Response: ', response);
        return res.status(200).send(response);
    } catch (err) {
        const message = `Unable to send amount: ${amount} from: ${sender} to: ${receiver}, due to error: ${err}`;
        const response = defaultResponse(true, message);
        logger.info('Response: ', response);
        return res.status(400).send(response);
    }
});

router.put('/balance', async (req, res) => {
    if (!req.body.address) {
        const response = defaultResponse(true, StringRes.MISSING_ADDRESS);
        logger.info('Response: ', response);
        return res.status(400).send(response);
    }

    const address = req.body.address;

    try {
        const balance = await _controller.getBalance(address);
        const response = defaultResponse(false, 'balance request', { balance: balance });
        logger.info('Response: ', response);
        return res.status(200).send(response);
    } catch (err) {
        const response = defaultResponse(true, 'unable to get balance');
        logger.info('Response: ', response);
        return res.status(400).send(response);
    }
});

const initialize = (controller: Controller) => {
    _controller = controller;
    return router
};

export = initialize;