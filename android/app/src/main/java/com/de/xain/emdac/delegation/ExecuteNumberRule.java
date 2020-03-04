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
