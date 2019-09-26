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

public class LocationRule extends Rule {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator<Rule>() {
        @Override
        public Rule createFromParcel(Parcel source) {
            return new LocationRule(source);
        }

        @Override
        public Rule[] newArray(int size) {
            return new Rule[size];
        }
    };

    @Expose
    private Float mLatitude;

    @Expose
    private Float mLongitude;

    @Expose
    private Float mRadius;

    @Expose
    private Unit mUnit;

    public LocationRule(Parcel parcel) {
        mId = (UUID) parcel.readSerializable();
        mLatitude = parcel.readFloat();
        mLongitude = parcel.readFloat();
        mRadius = parcel.readFloat();
        mUnit = Unit.values()[parcel.readInt()];
    }

    public LocationRule(Float latitude, Float longitude, Float radius, Unit unit) {
        mLatitude = latitude;
        mLongitude = longitude;
        mRadius = radius;
        mUnit = unit;
    }

    public Float getLatitude() {
        return mLatitude;
    }

    public void setLatitude(Float latitude) {
        mLatitude = latitude;
    }

    public Float getLongitude() {
        return mLongitude;
    }

    public void setLongitude(Float longitude) {
        mLongitude = longitude;
    }

    public Float getRadius() {
        return mRadius;
    }

    public void setRadius(Float radius) {
        mRadius = radius;
    }

    public Unit getUnit() {
        return mUnit;
    }

    public void setUnit(Unit unit) {
        mUnit = unit;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeSerializable(mId);
        dest.writeFloat(mLatitude);
        dest.writeFloat(mLongitude);
        dest.writeFloat(mRadius);
        dest.writeInt(mUnit.ordinal());
    }

    public enum Unit {
        KILOMETERS,
        MILES;

        public static Unit[] allValues() {
            return new Unit[]{KILOMETERS, MILES};
        }

        public String getShortStringValue() {
            switch (this) {
                case KILOMETERS:
                    return "km";
                case MILES:
                    return "mi";
                default:
                    return "";
            }
        }

        public Float toMeters(Float value) {
            switch (this) {
                case MILES:
                    return value * 1.60934f / 1000f;
                case KILOMETERS:
                default:
                    return value / 1000f;
            }
        }

        @Override
        public String toString() {
            switch (this) {
                case KILOMETERS:
                    return "kilometers";
                case MILES:
                    return "miles";
            }
            return super.toString();
        }
    }

    @Override
    public PolicyAttribute toPolicyAttribute() {

        StringBuilder builder = new StringBuilder();
        builder.append(mLatitude);
        builder.append(",");
        builder.append(mLongitude);
        builder.append(",");
        builder.append(mUnit.toMeters(mRadius));

        PolicyAttributeList policyAttributeList = new PolicyAttributeList();
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("geolocation", builder.toString()));
        policyAttributeList.addPolicyAttribute(new PolicyAttributeSingle("request.geolocation.type", "request.geolocation.value"));
        policyAttributeList.setOperation(PolicyAttributeList.Operation.GREATER_OR_EQUAL);

        return policyAttributeList;
    }
}
