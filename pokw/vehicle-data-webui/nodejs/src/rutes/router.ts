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
import { ControllerInterface } from '../controllers/controller_interface';
import moment from 'moment';

let _controller: ControllerInterface;

const router = Router();

router.get('/cat_last', async (req, res) => {
    const deviceId = '';
    // const deviceId = req.query.deviceId;
    // if (req.query.deviceId == undefined) {
    //     console.log(moment().format(), 'In query "deviceId" is missing');
    //     return res.status(400).send('deviceId missing');
    // }

    try {
        const last =  await _controller.getLast(deviceId);
        return res.status(200).send(last);
    } catch (err) {
        console.log(moment().format(), err);
        return res.status(400).send('Unable to get last message');
    }
});

const initializer = (controller: ControllerInterface): Router => {
    _controller = controller;
    return router;
}

export = initializer;
