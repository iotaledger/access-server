
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

import android.support.annotation.NonNull;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.math.BigInteger;
import java.security.NoSuchAlgorithmException;

/**
 * Model class representing a policy object
 */
public class Policy implements Serializable {

    @SerializedName("hash_function")
    @Expose
    private String hashFunction;

    @SerializedName("cost")
    @Expose
    private String cost;

    @SerializedName("policy_id")
    @Expose
    private String policyId;

    @SerializedName("policy_object")
    @Expose
    private PolicyObject policyObject = new PolicyObject();

    /**
     * No args constructor for use in serialization.
     */
    public Policy() {
    }

    private void calculatePolicyId() {
        try {
            byte[] data = policyObject.calculateHash(hashFunction);
            policyId = String.format("%0" + (data.length * 2) + "X", new BigInteger(1, data));
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
    }

    public String getCost() {
        return cost;
    }

    public void setCost(String cost) {
        this.cost = cost;
    }

    public void setCost(float cost) {
        if (cost == 0f) {
            this.cost = "0";
        } else {
            this.cost = String.valueOf(cost);
        }
    }

    public String getHashFunction() {
        return hashFunction;
    }

    public void setHashFunction(String hashFunction) {
        this.hashFunction = hashFunction;
    }

    public String getPolicyId() {
        return policyId;
    }

    public void setPolicyId(String policyId) {
        this.policyId = policyId;
    }

    public PolicyObject getPolicyObject() {
        return policyObject;
    }

    public void setPolicyObject(@NonNull PolicyObject policyObject) {
        this.policyObject = policyObject;
        calculatePolicyId();
    }
}
