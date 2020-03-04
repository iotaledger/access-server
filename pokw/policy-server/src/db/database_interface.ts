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

/**
 * Interface for quering the database
 */
export interface DatabaseInterface {
    /**
     * Get list of policies for device
     * 
     * @param deviceId ID of device
     */
    getPolicyListByDeviceId(deviceId: string): Promise<ReadonlyArray<Policy>>;

    /**
     * Get single policy by its ID
     * 
     * @param policyId ID of policy
     */
    getPolicyByPolicyId(policyId: string): Promise<Readonly<Policy>>;

    /**
     * Add new policy
     * 
     * @param policy Policy to be added
     */
    addNewPolicy(policy: Policy): Promise<void>

    /**
     * Delete all policies for device
     * 
     * @param deviceId ID of device
     */
    deletePoliciesForDeviceWithId(deviceId: string): Promise<void>
}

/**
 * Object representing policy inside database
 */
export interface Policy {
    hash?: string
    owner: string
    policyId: string
    deviceId: string
}
