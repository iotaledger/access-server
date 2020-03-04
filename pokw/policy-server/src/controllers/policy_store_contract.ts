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

const jsonInterface = require('../../build_sol/PolicyStore.json');
import { sha256 } from "js-sha256";
import { ethers } from 'ethers';
import _ from 'lodash';
import moment from 'moment';
const AsyncLock = require('async-lock');


/**
 * Object represenging policy inside contract
 */
export interface Policy {
    policyId: string,
    deviceId: string,
    owner: string,
    definition: object
}


/**
 * Class for communicating with policy store contract
 */
export class PolicyStoreContract {
    private contract?: ethers.Contract = undefined;
    private wallet: ethers.Wallet;
    private nonce: ethers.utils.BigNumber = ethers.utils.bigNumberify(0);
    private gasPrice: ethers.utils.BigNumber = ethers.utils.parseEther('1');
    private lock: any = new AsyncLock();

    constructor(providerUrl: string, ownerPrivateKey: string, contractAddress?: string) {
        const provider = new ethers.providers.JsonRpcProvider(providerUrl);

        this.wallet = new ethers.Wallet(ownerPrivateKey, provider);

        if (contractAddress) {
            this.contract = new ethers.Contract(
                contractAddress,
                jsonInterface.abi,
                this.wallet
            );

            // get initial nonce
            this.contract.provider.getTransactionCount(this.wallet.address)
                .then(nonce => {
                    this.nonce = ethers.utils.bigNumberify(nonce);
                    console.log('nonce:', nonce);
                })
                .catch(err => {
                    console.log('unable to get nonce');
                })

            // get gas price
            this.contract.provider.getGasPrice()
                .then(gasPrice => {
                    this.gasPrice = gasPrice;
                    console.log('gas price:', gasPrice);
                })
                .catch(err => {
                    console.log('unable to get gas price');
                });
        }
    }

    /**
     * Deploys contract to the blockchain
     * 
     * @returns {Promise<string>} Address of deployed contract
     */
    public async deploy(): Promise<string> {
        try {
            let factory = new ethers.ContractFactory(
                jsonInterface.abi,
                jsonInterface.bytecode,
                this.wallet
            );

            let contract = await factory.deploy();
            this.contract = await contract.deployed()

            return Promise.resolve(contract.address);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Creates new policy
     * 
     * @param policyId ID of policy
     * @param deviceId ID of device
     * @param owner Owner of policy
     * @param definition JSON represenging policy definition
     */
    public async createPolicy(
        policyId: string,
        deviceId: string,
        owner: string,
        definition: object): Promise<string | void>
    {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');

        console.log(moment().format(), 'Request: creating policy');

        // check if policy already exists
        try {
            const policy = await this.getPolicyByPolicyId(policyId);
            console.log('policy:', policy);
            return Promise.reject('Policy already exists');
        } catch (err) {
            // if policy does not exist, error is thrown which is ok
        }

        try {            
            // estimate gas limit
            const estimateGasLimit = await this.contract.estimate.createPolicy(
                policyId,
                owner,
                deviceId,
                JSON.stringify(definition)
            );

            const nonce = await this.getNonceAndIncrement();

            // override parameters
            const override = {
                nonce: nonce,
                gasPrice: this.gasPrice,
                gasLimit: estimateGasLimit.add(21000)
            }
            
            const transaction = await this.contract.functions.createPolicy(
                policyId,
                owner,
                deviceId,
                JSON.stringify(definition),
                override
            );

            // // await for transaction receipt
            // const receipt = await transaction.wait();
            console.log('Transaction:', transaction);
        } catch (err) {
            const errString = String(err);
            console.log(errString);
            if (errString.includes('known transaction')) {
                return Promise.resolve('Creating policy pending')
            } 
            else if (errString.includes('nonce has already been used')) {
                return this.createPolicy(policyId, deviceId, owner, definition);
            }
            else {
                return Promise.reject(err);
            }
        }

        return Promise.resolve();
    }

    /**
     * Gets policy with provided policy ID
     * 
     * @param policyId ID of policy
     */
    public async getPolicyByPolicyId(policyId: string): Promise<Policy> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        console.log(moment().format(), 'Request: get policy by ID:', policyId);
        try {
            let policy: Policy = {} as Policy;
            
            const result = await this.contract.functions.getPolicyByID(policyId);
            for (var prop in result) {
                if (Object.prototype.hasOwnProperty.call(result, prop)) {
                    if (result[prop] == '') return Promise.reject('Policy does not exist');
                }
            }

            if ('0' in result)
                policy.owner = result['0'];
            if ('1' in result)
                policy.deviceId = result['1'];
            if ('2' in result)
                policy.definition = JSON.parse(result['2']);
            policy.policyId = policyId;
            
            return Promise.resolve(policy);
        } catch (err) {
            console.log(err);
            return Promise.reject('Policy does not exist');
        }
    }

    /**
     * Delete all policies for device
     * 
     * @param deviceId ID of device
     */
    public async deletePoliciesForDeviceWithId(deviceId: string): Promise<string | void> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        console.log(moment().format(), 'Request: delete policies for device with ID:', deviceId);
        try {
            const count = await this.getPolicyCountForDevice(deviceId);
            if (count > 0) {
                try {
                    // estimate gas limit
                    const estimatedGasLimit = await this.contract.estimate
                        .deleteAllPoliciesForDevice(deviceId);

                    const nonce = await this.getNonceAndIncrement();

                    // override parameters
                    const override = {
                        nonce: nonce,
                        gasPrice: this.gasPrice,
                        gasLimit: estimatedGasLimit.add(21000)
                    }

                    const transaction = await this.contract.functions
                        .deleteAllPoliciesForDevice(deviceId, override);

                    // const receipt = await transaction.wait();
                    console.log('Transaction:', transaction);

                    return Promise.resolve();
                } catch (err) {
                    const errString = String(err);
                    if (errString.includes('known transaction')) {
                        return Promise.resolve('Deleting policies pending')
                    } else if (errString.includes('nonce has already been used')) {
                        return this.deletePoliciesForDeviceWithId(deviceId);
                    }
                    else {
                        return Promise.reject(err);
                    }
                }
            } else {
                return Promise.reject('There are no policies for device');
            }
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Get list of all policies for single device.
     * 
     * @param deviceId ID of device
     * 
     * @returns: List of all policies for single device.
     */
    public async getPolicyListForDeviceId(deviceId: string): Promise<ReadonlyArray<Policy>> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        console.log(moment().format(), 'Request: get policy list for device with ID:', deviceId);
        try {
            const count = await this.getPolicyCountForDevice(deviceId);
            if (count == 0)
                return Promise.resolve([]);
            
            const allPolicies: Array<Policy> = [];

            for (const index of _.range(count)) {
                const policyID = await this.getPolicyIdForDevice(deviceId, index);
                const policy = await this.getPolicyByPolicyId(policyID);
                allPolicies.push(policy);
            }

            return Promise.resolve(allPolicies);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Calculate policy store ID
     * 
     * @param policies Array of policies
     */
    public calculatePolicyStoreId(policies: ReadonlyArray<Policy>): string {
        // if (policies.length > 0) {
            const reducedPolicies = policies
                .map(policy => policy.definition)
                .reduce((acc, val) => acc + val, ''); 

            return '0x' + sha256(reducedPolicies);
        // } else {
        //     return '-1';
        // }
    }

    /**
     * Get count of policies for device
     * 
     * @param deviceId ID of device
     */
    public async getPolicyCountForDevice(deviceId: string): Promise<number> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        try {
            const result = await this.contract.functions.getPolicyCountForDevice(deviceId);
            return Promise.resolve(result);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Get policy ID with index 'index' inside array of policies for single device
     * 
     * @param deviceId ID of device
     * @param index index in array
     */
    public async getPolicyIdForDevice(deviceId: string, index: number): Promise<string> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        try {
            const result = await this.contract.functions.getPolicyIDForDevice(deviceId, index);
            return Promise.resolve(result);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    private getNonceAndIncrement(): Promise<number> {
        return this.lock.acquire('nonce', () => {
            const nonce = this.nonce;
            this.nonce = this.nonce.add(1);
            return Promise.resolve(nonce);
        });
    }
}
