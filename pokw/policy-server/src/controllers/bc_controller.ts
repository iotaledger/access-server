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

import { ControllerInterface, CommunicationType, Body, KnownCommands } from './controller_interface';
import _ from 'lodash';
import * as utils from '../utils/utils';
import { PolicyStoreContract } from './policy_store_contract';
import moment from 'moment';

export * from './controller_interface';

export class Controller implements ControllerInterface {
    public type: CommunicationType;
    private policyStore: PolicyStoreContract;

    constructor(
        provider: string,
        ownerPrivKey: string,
        contractAddr: string,
        type: CommunicationType)
    {
        this.type = type;
        this.policyStore = new PolicyStoreContract(
            provider,
            ownerPrivKey,
            contractAddr
          );
    }

    public async policy(body: Body): Promise<any> {
        if (_.isEmpty(body))
            return Promise.reject('missing body');
        const cmd: string = body.cmd;

        if (_.isEmpty(cmd))
            return Promise.reject('missing command');
        
        switch (cmd) {
            case KnownCommands.addPolicy:
                return this.addPolicy(body);

            case KnownCommands.clearPolicyList:
                return this.clearPolicyList(body);
            
            case KnownCommands.getPolicy:
                return this.getPolicy(body);

            case KnownCommands.getPolicyList:
                return this.getPolicyList(body);
                
            default:
                return Promise.reject('unsupported command'); 
        }
    }

    public async addPolicy(body: Body): Promise<any> {
        try {
            const deviceId = body.deviceId;
            const policy = body.policy;
            const owner = body.owner;
            
            if (_.isUndefined(owner)) return Promise.reject('missing owner');
            if (_.isUndefined(deviceId)) return Promise.reject('missing deviceId');
            if (_.isUndefined(policy)) return Promise.reject('missing policy');
            if (_.isUndefined(policy.policy_id)) 
                return Promise.reject('missing policy_id inside policy');

            let result = await this.policyStore.createPolicy(policy.policy_id, deviceId, owner, policy);
            let message: string;

            if (result != undefined) {
                message = result;
            } else {
                message = 'Policy added successfully';
            }

            console.log(moment().format(), message);

            if (this.type == CommunicationType.tcp)
                return Promise.resolve({
                    response: message
                });
            else
                return Promise.resolve(utils.defaultResponse(
                    false,
                    message
                ));
        } catch (err) {
            console.log(moment().format(), err);
            if (this.type == CommunicationType.tcp)
                return Promise.resolve({
                    response: err
                });
            else
                return Promise.resolve(utils.defaultResponse(
                    true,
                    err
                ));
        }
    }

    public async clearPolicyList(body: Body): Promise<any> {
        const deviceId = body.deviceId;
        if (_.isUndefined(deviceId)) return Promise.reject('missing deviceId');

        try {
            const result = await this.policyStore.deletePoliciesForDeviceWithId(deviceId);
            
            let message: string;

            if (result != undefined) {
                message = result;
            } else {
                message = 'Deleting all policies for device with ID: ' + deviceId;
            }
            console.log(moment().format(), 'Deleting all policies for device with ID: ' + deviceId);

            if (this.type == CommunicationType.tcp)
                return Promise.resolve({
                    response: message
                });
            else
                return Promise.resolve(utils.defaultResponse(
                    false,
                    message
                ));
        } catch (err) {
            console.log(moment().format(), 'Unable to delete policies for device with ID: ' +
                deviceId +
                ' due to error: ' +
                err);
            if (this.type == CommunicationType.tcp)
                return Promise.resolve({
                    response: 'Unable to delete policies for device with ID: ' + deviceId
                });
            else
                return Promise.resolve(utils.defaultResponse(
                    true,
                    'Unable to delete policies for device with ID: ' + deviceId
                ));
        }
    }
    
    public async getPolicy(body: Body): Promise<any> {
        const policyId = body.policyId;
        if (_.isUndefined(policyId)) return Promise.reject('missing policyId');

        try {
            const result = await this.policyStore.getPolicyByPolicyId(policyId);
            if (this.type == CommunicationType.tcp) {
                return Promise.resolve({ policy: result.definition });
            } else {
                return Promise.resolve(utils.defaultResponse(false, 'policy', result.definition));
            }
        } catch (err) {
            console.log(moment().format(), err);
            if (this.type == CommunicationType.tcp) {
                return Promise.resolve({ error: err });
            } else {
                return Promise.resolve(utils.defaultResponse(true, 'error', err));
            }
        }
    }

    public async getPolicyList(body: Body): Promise<any> {
        const deviceId = body.deviceId;
        const policyStoreId = body.policyStoreId;

        if (_.isUndefined(deviceId)) return Promise.reject('missing deviceId');
        if (_.isUndefined(policyStoreId)) return Promise.reject('missing policyStoreId');

        const policyList = await this.policyStore.getPolicyListForDeviceId(deviceId);
        const serverPolicyStoreId = this.policyStore.calculatePolicyStoreId(policyList);

        console.log(moment().format(), 'Get policy list:', policyList);

        if (serverPolicyStoreId === policyStoreId) {
            if (this.type == CommunicationType.tcp) {
                return Promise.resolve({ response: 'ok' });
            } else {
                return Promise.resolve(utils.defaultResponse(false, 'policy store not changed'));
            }
        } else {
            const policyIdList = policyList.map(policy => policy.policyId);

            if (this.type == CommunicationType.tcp)
                return Promise.resolve({
                    response: policyIdList,
                    policyStoreId: serverPolicyStoreId
                });
            else
                return Promise.resolve(utils.defaultResponse(
                    false,
                    'policy ID list',
                    {
                        list: policyIdList,
                        policyStoreId: serverPolicyStoreId
                    }
                ));
        }
    }
}
