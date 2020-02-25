export interface Controller {
    
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