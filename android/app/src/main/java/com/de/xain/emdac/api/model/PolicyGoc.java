
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

package com.de.xain.emdac.api.model;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.Iterator;
import java.util.List;

/**
 * Model class representing a policy goc object
 */
public class PolicyGoc implements Serializable {

    @SerializedName("attribute_list")
    @Expose
    private List<AttributeList> attributeList = null;
    @SerializedName("operation")
    @Expose
    private String operation;

    /**
     * No args constructor for use in serialization
     */
    public PolicyGoc() {
    }

    public List<AttributeList> getAttributeList() {
        return attributeList;
    }

    public String getOperation() {
        return operation;
    }

    public String toString() {
        StringBuffer returnValue = new StringBuffer();
        for (AttributeList temp : attributeList) {
            returnValue.append(temp.toString());
        }
        return returnValue.toString() + operation;
    }
}
