/**
 * Interface for quering the database
 */
export interface Database {
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