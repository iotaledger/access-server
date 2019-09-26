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
