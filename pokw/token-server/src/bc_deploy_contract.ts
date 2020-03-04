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

import { TokenContract } from './controllers/token_contract';
import moment from 'moment';
import path from 'path';
require('dotenv').config();

import config from 'config';

const main = async () => {

    let PROVIDER_IP_ADDR: string;
    if (config.has('provider.ipAddress')) {
        PROVIDER_IP_ADDR= config.get('provider.ipAddress');
    } else {
        console.log('Must define provider.ipAddress inside config file. Exiting...');
        return process.exit(1);
    }
   
    let PROVIDER_PORT_NUMBER: string;
    if (config.has('provider.portNumber')) {
        PROVIDER_PORT_NUMBER = config.get('provider.portNumber');
    } else {
        console.log('Must define provider.portNumber inside config file. Exiting...');
        return process.exit(1);
    }

    const ACCOUNT_PRIV_KEY = process.env.ACCOUNT_PRIV_KEY;
    if (ACCOUNT_PRIV_KEY === undefined) {
        console.log('Must provide ACCOUNT_PRIV_KEY as environment variable. Exiting...');
        return process.exit(1);
    }
    
    const providerUrl = 'http://' + PROVIDER_IP_ADDR + ':' + PROVIDER_PORT_NUMBER;
    
    const tokenContract = new TokenContract(
        providerUrl,
        ACCOUNT_PRIV_KEY);

    try {
        console.log(moment().format(), 'Deploying contract...');
        const contractAddress = await tokenContract.deploy();
        if (contractAddress) {
            console.log(moment().format(), 'Contract deployed to address:', contractAddress)
        } else {
            console.log(moment().format(), 'Unable to deploy contract!');
        }
    } catch (err) {
        console.log(moment().format(), 'Unable to deploy contract!', err);
    }
}

main();
