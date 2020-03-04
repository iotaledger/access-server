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
