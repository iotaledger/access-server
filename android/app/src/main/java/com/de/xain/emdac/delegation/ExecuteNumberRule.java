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

package com.de.xain.emdac.delegation;

import android.os.Parcel;
import android.os.Parcelable;

import com.de.xain.emdac.api.model.policy.PolicyAttribute;
import com.de.xain.emdac.api.model.policy.PolicyAttributeList;
import com.de.xain.emdac.api.model.policy.PolicyAttributeSingle;

import java.util.UUID;

public class ExecuteNumberRule extends Rule {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator<ExecuteNumberRule>() {
        @Override
        public ExecuteNumberRule createFromParcel(Parcel source) {
            return new ExecuteNumberRule(source);
        }

        @Override
        public ExecuteNumberRule[] newArray(int size) {
            return new ExecuteNumberRule[0];
        }
    };

    private Integer mMaxNumOfExecutions;

    private ExecuteNumberRule(Parcel parcel) {
        mId = (UUID) parcel.readSerializable();
        mMaxNumOfExecutions = parcel.readInt();
    }

    public ExecuteNumberRule(Integer maxNumOfExecutions) {
        mMaxNumOfExecutions = maxNumOfExecutions;
    }

    @Override
    public PolicyAttribute toPolicyAttribute() {
        PolicyAttributeList policyAttributeList;

        policyAttributeList = new PolicyAttributeList();
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("execution_num", String.valueOf(mMaxNumOfExecutions)));
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("request.execution_num.type", "request.execution_num.value"));
        policyAttributeList.setOperation(PolicyAttributeList.Operation.GREATER_THEN);

        return policyAttributeList;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeSerializable(mId);
        dest.writeInt(mMaxNumOfExecutions);
    }
}
