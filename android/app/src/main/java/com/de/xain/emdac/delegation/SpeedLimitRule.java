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

import java.util.UUID;

public class SpeedLimitRule extends Rule {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator<SpeedLimitRule>() {
        @Override
        public SpeedLimitRule createFromParcel(Parcel source) {
            return new SpeedLimitRule(source);
        }

        @Override
        public SpeedLimitRule[] newArray(int size) {
            return new SpeedLimitRule[size];
        }
    };

    @Expose
    private Float mSpeedLimit;

    @Expose
    private Unit mUnit;

    private SpeedLimitRule(Parcel source) {
        mId = (UUID) source.readSerializable();
        mSpeedLimit = source.readFloat();
        mUnit = Unit.values()[source.readInt()];
    }

    public SpeedLimitRule(Float speedLimit, Unit unit) {
        mSpeedLimit = speedLimit;
        mUnit = unit;
    }

    public Float getSpeedLimit() {
        return mSpeedLimit;
    }

    public void setSpeedLimit(Float speedLimit) {
        mSpeedLimit = speedLimit;
    }

    public Unit getUnit() {
        return mUnit;
    }

    public void setUnit(Unit unit) {
        mUnit = unit;
    }

    @Override
    public PolicyAttribute toPolicyAttribute() {
        PolicyAttributeList policyAttributeList;

        policyAttributeList = new PolicyAttributeList();
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("speed", String.valueOf(mUnit.toMetric(mSpeedLimit))));
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("request.speed.type", "request.speed.value"));
        policyAttributeList.setOperation(PolicyAttributeList.Operation.GREATER_OR_EQUAL);

        return policyAttributeList;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeSerializable(mId);
        dest.writeFloat(mSpeedLimit);
        dest.writeInt(mUnit.ordinal());
    }

    public enum Unit {
        METRIC,
        IMPERIAL;

        public static Unit[] allValues() {
            return new Unit[]{METRIC, IMPERIAL};
        }

        public float toMetric(Float value) {
            switch (this) {
                case METRIC:
                    return value;
                case IMPERIAL:
                default:
                    return value * 0.621371f;
            }
        }

        @Override
        public String toString() {
            switch (this) {
                case METRIC:
                    return "km/h";
                case IMPERIAL:
                    return "mph";
                default:
                    return "";
            }
        }
    }
}
