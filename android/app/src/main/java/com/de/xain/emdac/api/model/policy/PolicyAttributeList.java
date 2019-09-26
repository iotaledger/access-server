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
