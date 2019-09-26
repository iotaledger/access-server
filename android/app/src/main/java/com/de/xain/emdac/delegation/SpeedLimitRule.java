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
