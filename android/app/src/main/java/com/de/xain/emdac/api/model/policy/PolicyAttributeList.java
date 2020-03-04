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

import java.util.ArrayList;
import java.util.List;

public class PolicyAttributeList extends PolicyAttribute {

    @SerializedName("attribute_list")
    @Expose
    private List<PolicyAttribute> attributeList;
    @SerializedName("operation")
    @Expose
    private Operation operation;

    /**
     * No args constructor for use in serialization
     */
    public PolicyAttributeList() {
    }

    public PolicyAttributeList(List<PolicyAttribute> attributeList, Operation operation) {
        this.attributeList = attributeList;
        this.operation = operation;
    }

    public void addPolicyAttribute(PolicyAttribute policyAttribute) {
        if (attributeList == null)
            attributeList = new ArrayList<>();
        attributeList.add(policyAttribute);
    }

    public void setOperation(Operation operation) {
        this.operation = operation;
    }

    public enum Operation {
        @SerializedName("eq") EQUAL,
        @SerializedName("leq") LESS_OR_EQUAL,
        @SerializedName("geq") GREATER_OR_EQUAL,
        @SerializedName("lt") LESS_THAN,
        @SerializedName("gt") GREATER_THEN,
        @SerializedName("and") AND,
        @SerializedName("or") OR;

        @Override
        public String toString() {
            switch (this) {
                case EQUAL:
                    return "eq";
                case LESS_OR_EQUAL:
                    return "leq";
                case GREATER_OR_EQUAL:
                    return "geq";
                case LESS_THAN:
                    return "lt";
                case GREATER_THEN:
                    return "gt";
                case AND:
                    return "and";
                case OR:
                    return "or";
            }
            return "";
        }
    }

}
