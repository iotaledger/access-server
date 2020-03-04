
/*
 *  This file is part of the Frost distribution
 *  (https://github.com/xainag/frost)
 *
 *  Copyright (c) 2019 XAIN AG.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package com.de.xain.emdac.api.model.policy;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Model class representing policy object
 */
public class PolicyObject implements Serializable {

    @SerializedName("obligation_deny")
    @Expose
    private PolicyAttribute obligationDeny;

    @SerializedName("obligation_grant")
    @Expose
    private PolicyAttribute obligationGrant;

    @SerializedName("policy_doc")
    @Expose
    private PolicyAttribute policyDoc;

    @SerializedName("policy_goc")
    @Expose
    private PolicyAttribute policyGoc;

    /**
     * No args constructor for use in serialization
     */
    public PolicyObject() {
    }

    public byte[] calculateHash(String hashFunction) throws NoSuchAlgorithmException {
        MessageDigest digest = MessageDigest.getInstance(hashFunction);
        digest.reset();
        return digest.digest(this.toString().getBytes());
    }

    public PolicyAttribute getObligationDeny() {
        return obligationDeny;
    }

    public void setObligationDeny(PolicyAttribute obligationDeny) {
        this.obligationDeny = obligationDeny;
    }

    public PolicyAttribute getObligationGrant() {
        return obligationGrant;
    }

    public void setObligationGrant(PolicyAttribute obligationGrant) {
        this.obligationGrant = obligationGrant;
    }

    public PolicyAttribute getPolicyDoc() {
        return policyDoc;
    }

    public void setPolicyDoc(PolicyAttribute policyDoc) {
        this.policyDoc = policyDoc;
    }

    public PolicyAttribute getPolicyGoc() {
        return policyGoc;
    }

    public void setPolicyGoc(PolicyAttribute policyGoc) {
        this.policyGoc = policyGoc;
    }

    public String toString() {
        return policyDoc.toString() + policyGoc.toString();
    }
}
