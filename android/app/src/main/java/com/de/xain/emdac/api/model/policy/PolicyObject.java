
/*
 *  This file is part of the DAC distribution (https://github.com/xainag/frost)
 *  Copyright (c) 2019 XAIN AG.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
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
