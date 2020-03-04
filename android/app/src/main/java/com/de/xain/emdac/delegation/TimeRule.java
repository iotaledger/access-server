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
import com.google.gson.annotations.Expose;

import java.util.Date;
import java.util.UUID;

public class TimeRule extends Rule {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator<TimeRule>() {
        @Override
        public TimeRule createFromParcel(Parcel source) {
            return new TimeRule(source);
        }

        @Override
        public TimeRule[] newArray(int size) {
            return new TimeRule[size];
        }
    };

    @Expose
    private Date mFromDate;

    @Expose
    private Date mUntilDate;

    private TimeRule(Parcel parcel) {
        mId = (UUID) parcel.readSerializable();
        mFromDate = new Date(parcel.readLong());
        mUntilDate = new Date(parcel.readLong());
    }

    public TimeRule(Date fromDate, Date untilDate) {
        mFromDate = fromDate;
        mUntilDate = untilDate;
    }

    public Date getFromDate() {
        return mFromDate;
    }

    public void setFromDate(Date fromDate) {
        mFromDate = fromDate;
    }

    public Date getUntilDate() {
        return mUntilDate;
    }

    public void setUntilDate(Date untilDate) {
        mUntilDate = untilDate;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeSerializable(mId);
        dest.writeLong(mFromDate.getTime());
        dest.writeLong(mUntilDate.getTime());
    }

    @Override
    public PolicyAttribute toPolicyAttribute() {
        PolicyAttributeList finalList = new PolicyAttributeList();
        PolicyAttributeList policyAttributeList;

        // from date
        policyAttributeList = new PolicyAttributeList();
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("time", String.valueOf((int) (mFromDate.getTime() / 1000))));
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("request.time.type", "request.time.value"));
        policyAttributeList.setOperation(PolicyAttributeList.Operation.LESS_OR_EQUAL);
        finalList.addPolicyAttribute(policyAttributeList);

        // until date
        policyAttributeList = new PolicyAttributeList();
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("time", String.valueOf((int) (mUntilDate.getTime() / 1000))));
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("request.time.type", "request.time.value"));
        policyAttributeList.setOperation(PolicyAttributeList.Operation.GREATER_OR_EQUAL);
        finalList.addPolicyAttribute(policyAttributeList);

        finalList.setOperation(PolicyAttributeList.Operation.AND);

        return finalList;
    }
}
