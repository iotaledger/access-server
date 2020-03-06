/*
 * This file is part of the Frost distribution
 * (https://github.com/iotaledger/frost)
 * 
 * Copyright (c) 2020 IOTA.
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

import _ from 'lodash';
import { Database, Policy } from './database';
import logger from '../utils/logger';
const pgp = require('pg-promise')();
require('dotenv').config();
import config from 'config';


/**
 * Class for communicating with PostgreSQL database
 */
export class PgDatabase implements Database {

    private db: any;

    constructor() {
        const DB_USER = process.env.POSTRGES_USER;
        if (DB_USER == undefined)
            throw 'User for postgres has not been defined. Please define it in .env file under POSTRGES_USER';

        let DB_HOST: string;
        if (config.has('db.host'))
            DB_HOST = config.get('db.host');
        else
            throw 'Host for postgres has not been defined. Please define it in config file under db.host';

        const DB_DATABASE = process.env.POSTGRES_DB;
        if (DB_DATABASE == undefined)
            throw 'Database for postgres has not been defined. Please define it in .env file under POSTGRES_DB';

        let DB_PORT: string;
        if (config.has('db.port'))
            DB_PORT = config.get('db.port');
        else
            throw 'Port number for postgres has not been defined. Please define it in config file under db.port';

        const DB_PASSWORD = process.env.POSTGRES_PASSWORD;
        if (DB_PASSWORD == undefined)
            throw 'Password for postgres has not been defined. Please define it in .env file under POSTGRES_PASSWORD';

        const cn = {
            user: DB_USER,
            host: DB_HOST,
            database: DB_DATABASE,
            password: DB_PASSWORD,
            port: DB_PORT,
        };
        this.db = pgp(cn);

        // try to connect to database
        this.db.connect()
            .then((obj: any) => {
                logger.info('Successfully connected to the database');
                obj.done();
            })
            .catch((error: any) => {
                logger.error('Unable to connect to the database: ', error);
            })
    }

    async getPolicyListByDeviceId(deviceId: string): Promise<readonly Policy[]> {
        try {
            const result: ReadonlyArray<Policy> = await this.db
                .any('SELECT * FROM policy WHERE "deviceId"=$1', [deviceId]);

            return Promise.resolve(result);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    async getPolicyByPolicyId(policyId: string): Promise<Readonly<Policy>> {
        try {
            const res = await this.db.any('SELECT * FROM policy WHERE "policyId"=$1 LIMIT 1', [policyId]);
            if (_.isEmpty(res))
                return Promise.reject(`Unable to find policy with policyId: ${policyId}`);
            else
                return Promise.resolve(res[0]);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    async addNewPolicy(policy: Policy): Promise<void> {
        try {
            await this.db.query('INSERT INTO policy(hash,owner,"policyId","deviceId") VALUES($1,$2,$3,$4)',
                [policy.hash, policy.owner, policy.policyId, policy.deviceId]);

            return Promise.resolve();
        } catch (err) {
            return Promise.reject(err);
        }
    }

    async deletePoliciesForDeviceWithId(deviceId: string): Promise<void> {
        try {
            await this.db.query('DELETE FROM policy WHERE "deviceId"=$1', [deviceId]);
            return Promise.resolve();
        } catch (err) {
            return Promise.reject(err);
        }
    }
}
