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

const jsonInterface = require('../../build_sol/contracts/NTTToken.json');
import { ethers } from 'ethers';
import * as utils from '../utils/utils';
const AsyncLock = require('async-lock');

/**
 * Class for cummunicating with token contract
 */
export class TokenContract {
    private decimals: number = 18;
    private contract?: ethers.Contract = undefined;
    private wallet: ethers.Wallet;
    private nonce: ethers.utils.BigNumber = ethers.utils.bigNumberify(0);
    private lock: any = new AsyncLock();

    /**
     * Constructs class for communicating with token contract
     * 
     * @param providerUrl URL of provider (eth node)
     * @param ownerPrivateKey Private key of contract's owner (one who has deployed contract)
     * @param contractAddress Address of contract (if it is deployed)
     */
    constructor(providerUrl: string, ownerPrivateKey: string, contractAddress?: string) {
        const provider = new ethers.providers.JsonRpcProvider(providerUrl);

        this.wallet = new ethers.Wallet(ownerPrivateKey, provider);

        if (contractAddress) {
            this.contract = new ethers.Contract(contractAddress, jsonInterface.abi, this.wallet);
            this.getDecimals();

            this.contract.provider.getTransactionCount(this.wallet.address)
                .then(nonce => {
                    this.nonce = ethers.utils.bigNumberify(nonce);
                    console.log('nonce', nonce);
                });
        }
    }

    /**
     * Deploy contract to the blockchain
     * 
     * @returns {Promise<string>} Address of deployed contract
     */
    public async deploy(): Promise<string> {
        try {
            let factory = new ethers.ContractFactory(jsonInterface.abi, jsonInterface.bytecode, this.wallet);
        
            let contract = await factory.deploy('Token', 'MYTOK', '3', String(ethers.utils.parseEther('1000000')));

            this.contract = await contract.deployed()

            return Promise.resolve(contract.address);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Transfer certain amount of tokens from sender to receiver
     * 
     * @param sender Sender of tokens
     * @param receiver Receiver of tokens
     * @param amount Amount of tokens to be transfered
     */
    public async transferFrom(sender: string, receiver: string, amount: number): Promise<string | void> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        console.log('Request: transfer from');
        const convertedAmount = utils.toTokenWithoutDecimals(amount, this.decimals);
        try {
            const _sender = ethers.utils.getAddress(sender);
            const _receiver = ethers.utils.getAddress(receiver);

            // check does sender has sufficient funds
            const balance = await this.getBalance(_sender);
            const convertedBalance = utils.toTokenWithoutDecimals(Number(balance), this.decimals);
            console.log('Balance:', balance, 'Amount:', amount);
            console.log('ConvertedBalance:', convertedBalance, 'ConvertedAmount:', convertedAmount);
            if (convertedAmount > convertedBalance) {
                return Promise.reject('Insufficient funds');
            }

            // estimate gas limit
            const gasLimit = await this.contract.estimate.transferFrom(_sender, _receiver, convertedAmount);

            const nonce = await this.getNonceAndIncrement();

            // override parameters
            const parameters = {
                nonce: nonce,
                gasPrice: ethers.utils.parseEther('1'),
                gasLimit: gasLimit.add(21000)
            }

            // call contract's non constant function
            const transaction = await this.contract.functions
                .transferFrom(_sender, _receiver, convertedAmount, parameters)

            // await for recept
            // const receipt = await transaction.wait();

            // console.log(receipt);
            return Promise.resolve();
        } catch (err) {
            const errString = String(err);
            console.log(errString);
            if (errString.includes('known transaction')) {
                return Promise.resolve('Pending');
            }
            else if (errString.includes('nonce has already been used')) {
                return this.transferFrom(sender, receiver, amount);
            }
            else {
                return Promise.reject(err);
            }
        }
    }

    /**
     * Transfer certain amount of tokens to provided address
     * 
     * @param receiver Receiver of tokens
     * @param amount Amount of tokens to be transfered
     */
    public async transfer(receiver: string, amount: number): Promise<void | string> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        const convertedAmount = utils.toTokenWithoutDecimals(amount, this.decimals);
        try {
            const _receiver = ethers.utils.getAddress(receiver);

            // estimate gas limit
            const gasLimit = await this.contract.estimate.transfer(_receiver, convertedAmount);

            const nonce = await this.getNonceAndIncrement();

            // override parameters
            const parameters = {
                nonce: nonce,
                gasPrice: ethers.utils.parseEther('1'),
                gasLimit: gasLimit.add(21000)
            }

            // call contract's non constant function
            const transaction = await this.contract.functions
                .transfer(_receiver, convertedAmount, parameters);            

            // // await for receipt
            // const receipt = await transaction.wait();

            // console.log(receipt);
            return Promise.resolve();
        } catch (err) {
            const errString = String(err);
            console.log(errString);
            if (errString.includes('known transaction')) {
                return Promise.resolve('Pending');
            }
            else if (errString.includes('nonce has already been used')) {
                return this.transfer(receiver, amount);
            }
            else {
                return Promise.reject(err);
            }
        }
    }

    /**
     * Gets balance of user with provided address
     * 
     * @param address Address of user
     */
    public async getBalance(address: string): Promise<string> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        try {
            const _address = ethers.utils.getAddress(address);
            const balance = await this.contract.functions.balanceOf(_address);
            return Promise.resolve(utils.toTokenWithDecimals(balance, this.decimals));
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Get number of decimals used for better human readability
     */
    public async getDecimals(): Promise<number> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        try {
            const decimals = await this.contract.functions.decimals();
            if (decimals) this.decimals = decimals;
            return Promise.resolve(decimals);
        } catch (err) {
            return Promise.reject(err);
        }
    }

    /**
     * Get total supply of tokens
     */
    public async getTotalSupply(): Promise<number> {
        if (this.contract === undefined) return Promise.reject('Contract not deployed');
        try {
            const totalSupply = this.contract.functions.totalSupply();
            return Promise.resolve(totalSupply);
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
