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
