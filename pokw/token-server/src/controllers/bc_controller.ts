/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
