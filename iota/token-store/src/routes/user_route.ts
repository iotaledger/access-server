import { Router } from 'express';
import * as utils from '../utils/utils';
import _ from 'lodash';
import { Controller } from '../controllers/controller';
import logger from '../utils/logger';

let _controller: Controller;

const router = Router();

router.put('/account', async function(req, res) {
    let address;

    try {
        address = await _controller.createAccount();
        logger.info('Account successfully created with address:', address);
    } catch (err) {
        logger.error('Unable to create account with error:', err);
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

const initialize = (controller: Controller) => { 
    _controller = controller;
    return router
};

export = initialize;