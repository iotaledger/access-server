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
