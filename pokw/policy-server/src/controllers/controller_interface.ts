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

export interface ControllerInterface {
    
    type: CommunicationType

    /**
     * Adds new policy
     * 
     * @param body Request body
     */
    addPolicy(body: Body): Promise<any>

    /**
     * Clears list of policy
     * 
     * @param body Request body
     */
    clearPolicyList(body: Body): Promise<any>

    /**
     * Gets single policy
     * 
     * @param body Request body
     */
    getPolicy(body: Body): Promise<any>

    /**
     * Gets list of policies
     * 
     * @param body Request body
     */
    getPolicyList(body: Body): Promise<any>
}

/**
 * Interface represenging body of request
 */
export interface Body {
    cmd: string
    deviceId?: string
    owner?: string
    policy?: any
    policyStoreId?: string
    policyId?: string
}

/**
 * Type of server
 */
export enum CommunicationType {
    tcp = 'tcp',
    rest = 'rest'
}

export enum KnownCommands {
    addPolicy = 'add_policy',
    clearPolicyList = 'clear_policy_list',
    getPolicy = 'get_policy',
    getPolicyList = 'get_policy_list'
}
