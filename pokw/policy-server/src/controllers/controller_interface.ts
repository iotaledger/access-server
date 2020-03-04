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
