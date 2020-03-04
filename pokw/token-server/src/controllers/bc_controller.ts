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

import { ControllerInterface } from './controller_interface';
import { ethers } from 'ethers';
import { TokenContract } from './token_contract';

export class Controller implements ControllerInterface {
    private tokenContract: TokenContract;

    /**
     * @param providerUrl URL of provider (eth node)
     * @param ownerPrivateKey Private key of contract's owner (one who has deployed contract)
     * @param contractAddress Address of contract (if it is deployed)
     */
    constructor(providerUrl: string, ownerPrivateKey: string, contractAddress?: string) {
        this.tokenContract = new TokenContract(providerUrl, ownerPrivateKey, contractAddress);
    }
    
    async createAccount(): Promise<string> {
        try {
            const newAccount = await ethers.Wallet.createRandom().getAddress();
            return Promise.resolve(newAccount);
        } catch (err) {
            return Promise.reject(err)
        }
    }

    getBalance(address: string): Promise<string> {
        return this.tokenContract.getBalance(address)
    }

    send(from: string, to: string, amount: number): Promise<string | void> {
        return this.tokenContract.transferFrom(from, to, amount);
    }
    
    fund(receiver: string, amount: number): Promise<string | void> {
        return this.tokenContract.transfer(receiver, amount);
    }
}
