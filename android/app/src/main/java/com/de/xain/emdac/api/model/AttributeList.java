
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

package com.de.xain.emdac.api.model;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.List;

/**
 * Model class representing an PolicyAttributeList
 */
public class AttributeList implements Serializable {

    @SerializedName("attribute_list")
    @Expose
    private List<AttributeList_> attributeList = null;
    @SerializedName("operation")
    @Expose
    private String operation;

    /**
     * No args constructor for use in serialization
     */
    public AttributeList() {
    }

    public List<AttributeList_> getAttributeList() {
        return attributeList;
    }

    public String getOperation() {
        return operation;
    }

    public String toString() {
        StringBuffer returnValue = new StringBuffer();
        for (AttributeList_ temp : attributeList) {
            returnValue.append(temp.toString());
        }
        return returnValue.toString() + operation;
    }
}
