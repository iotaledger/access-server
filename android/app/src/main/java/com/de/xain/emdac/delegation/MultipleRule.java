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
import com.google.gson.annotations.Expose;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class MultipleRule extends Rule {

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator<MultipleRule>() {
        @Override
        public MultipleRule createFromParcel(Parcel source) {
            return new MultipleRule(source);
        }

        @Override
        public MultipleRule[] newArray(int size) {
            return new MultipleRule[size];
        }
    };

    @Expose
    private List<Rule> mRuleList;

    @Expose
    private RuleSatisfyType mRuleSatisfyType;

    public MultipleRule(Parcel parcel) {
        mId = (UUID) parcel.readSerializable();
        mRuleList = new ArrayList<>();
        parcel.readList(mRuleList, Rule.class.getClassLoader());
    }

    public MultipleRule(List<Rule> ruleList, RuleSatisfyType ruleSatisfyType) {
        mRuleList = ruleList;
        mRuleSatisfyType = ruleSatisfyType;
    }

    public List<Rule> getRuleList() {
        return mRuleList;
    }

    public void setRuleList(List<Rule> ruleList) {
        mRuleList = ruleList;
    }

    public RuleSatisfyType getRuleSatisfyType() {
        return mRuleSatisfyType;
    }

    public void setRuleSatisfyType(RuleSatisfyType ruleSatisfyType) {
        mRuleSatisfyType = ruleSatisfyType;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeSerializable(mId);
        dest.writeList(mRuleList);
        dest.writeInt(mRuleSatisfyType.ordinal());
    }

    @Override
    public PolicyAttribute toPolicyAttribute() {
        PolicyAttributeList policyAttributeList = new PolicyAttributeList();
        for (Rule rule : mRuleList) {
            policyAttributeList.addPolicyAttribute(rule.toPolicyAttribute());
        }
        policyAttributeList.setOperation(mRuleSatisfyType.toOperation());

        return policyAttributeList;
    }
}
