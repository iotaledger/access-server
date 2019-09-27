"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
const express_1 = require("express");
const moment_1 = __importDefault(require("moment"));
let _controller;
const router = express_1.Router();
router.get('/cat_last', async (req, res) => {
    const deviceId = '';
    // const deviceId = req.query.deviceId;
    // if (req.query.deviceId == undefined) {
    //     console.log(moment().format(), 'In query "deviceId" is missing');
    //     return res.status(400).send('deviceId missing');
    // }
    try {
        const last = await _controller.getLast(deviceId);
        return res.status(200).send(last);
    }
    catch (err) {
        console.log(moment_1.default().format(), err);
        return res.status(400).send('Unable to get last message');
    }
});
const initializer = (controller) => {
    _controller = controller;
    return router;
};
module.exports = initializer;
