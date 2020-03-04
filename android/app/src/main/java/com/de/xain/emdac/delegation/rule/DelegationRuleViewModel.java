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

package com.de.xain.emdac.delegation.rule;

import android.databinding.Bindable;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.de.xain.emdac.BR;
import com.de.xain.emdac.BaseObservableViewModel;
import com.de.xain.emdac.R;
import com.de.xain.emdac.SettingsFragment;
import com.de.xain.emdac.delegation.LocationRule;
import com.de.xain.emdac.delegation.MultipleRule;
import com.de.xain.emdac.delegation.Rule;
import com.de.xain.emdac.delegation.RuleLimitation;
import com.de.xain.emdac.delegation.RuleSatisfyType;
import com.de.xain.emdac.delegation.RuleType;
import com.de.xain.emdac.delegation.SpeedLimitRule;
import com.de.xain.emdac.delegation.TimeRule;
import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.utils.ResourceProvider;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.UUID;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.subjects.BehaviorSubject;
import io.reactivex.subjects.PublishSubject;

@SuppressWarnings("WeakerAccess")
public class DelegationRuleViewModel extends BaseObservableViewModel {

    public static final RuleType[] RULE_TYPES = RuleType.getAllValues();
    public static final RuleSatisfyType[] RULE_SATISFY_TYPES = RuleSatisfyType.getAllValues();
    public static final RuleLimitation[] RULE_LIMITATIONS = RuleLimitation.getAllValues();

    private final ResourceProvider mResourceProvider;
    private final BehaviorSubject<List<Rule>> mRuleList = BehaviorSubject.createDefault(new ArrayList<>());
    private final PublishSubject<String> mShowMessage = PublishSubject.create();
    private final BehaviorSubject<RuleType> mRuleType = BehaviorSubject.createDefault(RuleType.SINGLE);
    private final BehaviorSubject<RuleSatisfyType> mRuleSatisfyType = BehaviorSubject.createDefault(RuleSatisfyType.ONE_OR_MORE);
    private final BehaviorSubject<RuleLimitation> mRuleLimitation = BehaviorSubject.createDefault(RuleLimitation.TIME);
    private final BehaviorSubject<Date> mLimitationTimeFrom;
    private final BehaviorSubject<Date> mLimitationTimeUntil;
    private String mSpeedLimit;
    private String mLatitude;
    private String mLongitude;
    private String mRadius;
    private LocationRule.Unit mLocationUnit;
    private SpeedLimitRule.Unit mSpeedLimitUnit;
    private Rule mRule;

    private int mSelectedUnitIndex = 0;
    private int mSelectedRuleTypeIndex = 0;
    private int mSelectedRuleSatisfyTypeIndex = 0;
    private int mSelectedRuleLimitationIndex = 0;

    @Inject
    public DelegationRuleViewModel(AppSharedPreferences preferences, ResourceProvider resourceProvider) {
        mResourceProvider = resourceProvider;
        Calendar date = Calendar.getInstance();
        mLimitationTimeFrom = BehaviorSubject.createDefault(date.getTime());

        Calendar untilTime = Calendar.getInstance();
        untilTime.setTime(date.getTime());
        untilTime.add(Calendar.HOUR, 1);
        mLimitationTimeUntil = BehaviorSubject.createDefault(untilTime.getTime());

        mLocationUnit = LocationRule.Unit.KILOMETERS;
        mSpeedLimitUnit = SpeedLimitRule.Unit.METRIC;
        String savedDistanceUnit = preferences.getString(SettingsFragment.Keys.PREF_KEY_DISTANCE_UNIT);
        try {
            mLocationUnit = LocationRule.Unit.values()[Integer.valueOf(savedDistanceUnit)];
            mSpeedLimitUnit = SpeedLimitRule.Unit.values()[Integer.valueOf(savedDistanceUnit)];
        } catch (NumberFormatException ignored) {
        }
    }

    @Bindable
    public int getSelectedUnitIndex() {
        return mSelectedUnitIndex;
    }

    public void setSelectedUnitIndex(int selectedUnitIndex) {
        mSelectedUnitIndex = selectedUnitIndex;
        notifyPropertyChanged(BR.selectedUnitIndex);
    }

    @Bindable
    public String getSpeedLimit() {
        return mSpeedLimit;
    }

    public void setSpeedLimit(String speedLimit) {
        mSpeedLimit = speedLimit;
    }

    @Bindable
    public int getSelectedRuleTypeIndex() {
        return mSelectedRuleTypeIndex;
    }

    public void setSelectedRuleTypeIndex(int selectedRuleTypeIndex) {
        mSelectedRuleTypeIndex = selectedRuleTypeIndex;
        notifyPropertyChanged(BR.selectedRuleTypeIndex);
        mRuleType.onNext(RULE_TYPES[selectedRuleTypeIndex]);
    }

    @Bindable
    public int getSelectedRuleSatisfyTypeIndex() {
        return mSelectedRuleSatisfyTypeIndex;
    }

    public void setSelectedRuleSatisfyTypeIndex(int selectedRuleSatisfyTypeIndex) {
        mSelectedRuleSatisfyTypeIndex = selectedRuleSatisfyTypeIndex;
        notifyPropertyChanged(BR.selectedRuleSatisfyTypeIndex);
        mRuleSatisfyType.onNext(RULE_SATISFY_TYPES[selectedRuleSatisfyTypeIndex]);
    }

    @Bindable
    public int getSelectedRuleLimitationIndex() {
        return mSelectedRuleLimitationIndex;
    }

    public void setSelectedRuleLimitationIndex(int selectedRuleLimitationIndex) {
        mSelectedRuleLimitationIndex = selectedRuleLimitationIndex;
        notifyPropertyChanged(BR.selectedRuleLimitationIndex);
        mRuleLimitation.onNext(RULE_LIMITATIONS[selectedRuleLimitationIndex]);
    }

    public boolean isEditingExistingRule() {
        return mRule != null;
    }

    public Observable<String> getShowMessage() {
        return mShowMessage;
    }

    public Observable<List<Rule>> getObservableRuleList() {
        return mRuleList;
    }

    @Nullable
    public Rule getRule() {
        return mRule;
    }

    public void setInitialRule(Rule rule) {
        if (mRule == null) {
            mRule = rule;
        } else {
            return;
        }

        int ruleTypeIndex = 0;
        int ruleSatisfyTypeIndex = 0;
        int ruleLimitationIndex = 0;

        if (rule instanceof TimeRule) {
            TimeRule timeRule = (TimeRule) rule;
            ruleTypeIndex = Arrays.asList(RULE_TYPES).indexOf(RuleType.SINGLE);
            ruleLimitationIndex = Arrays.asList(RULE_LIMITATIONS).indexOf(RuleLimitation.TIME);
            mLimitationTimeFrom.onNext(timeRule.getFromDate());
            mLimitationTimeUntil.onNext(timeRule.getUntilDate());
        } else if (rule instanceof LocationRule) {
            LocationRule locationRule = (LocationRule) rule;
            ruleTypeIndex = Arrays.asList(RULE_TYPES).indexOf(RuleType.SINGLE);
            ruleLimitationIndex = Arrays.asList(RULE_LIMITATIONS).indexOf(RuleLimitation.LOCATION);
            setLatitude(locationRule.getLatitude().toString());
            setLongitude(locationRule.getLongitude().toString());
            setRadius(locationRule.getRadius().toString());
            mLocationUnit = locationRule.getUnit();
        } else if (rule instanceof  SpeedLimitRule) {
            SpeedLimitRule speedLimitRule = (SpeedLimitRule) rule;
            ruleTypeIndex = Arrays.asList(RULE_TYPES).indexOf(RuleType.SINGLE);
            ruleLimitationIndex = Arrays.asList(RULE_LIMITATIONS).indexOf(RuleLimitation.SPEED_LIMIT);
            setSpeedLimit(speedLimitRule.getSpeedLimit().toString());
            mSpeedLimitUnit = speedLimitRule.getUnit();
        } else if (rule instanceof MultipleRule) {
            MultipleRule multipleRule = (MultipleRule) rule;
            ruleTypeIndex = Arrays.asList(RULE_TYPES).indexOf(RuleType.MULTIPLE);
            mRuleSatisfyType.onNext(multipleRule.getRuleSatisfyType());
            mRuleList.onNext(multipleRule.getRuleList());
        }

        setSelectedRuleTypeIndex(ruleTypeIndex);
        setSelectedRuleLimitationIndex(ruleLimitationIndex);
        setSelectedRuleSatisfyTypeIndex(ruleSatisfyTypeIndex);
    }

    public void addRule(Rule rule) {
        List<Rule> ruleList = mRuleList.getValue();
        ruleList.add(rule);
        mRuleList.onNext(ruleList);
    }

    public void removeRule(Rule rule) {
        List<Rule> ruleList = mRuleList.getValue();
        ruleList.remove(rule);
        mRuleList.onNext(ruleList);
    }

    @Bindable
    public String getLatitude() {
        return mLatitude;
    }

    public void setLatitude(String latitude) {
        mLatitude = latitude;
        notifyPropertyChanged(BR.latitude);
    }

    @Bindable
    public String getLongitude() {
        return mLongitude;
    }

    public void setLongitude(String longitude) {
        mLongitude = longitude;
        notifyPropertyChanged(BR.longitude);
    }

    @Bindable
    public String getRadius() {
        return mRadius;
    }

    public void setRadius(String radius) {
        mRadius = radius;
        notifyPropertyChanged(BR.radius);
    }

    public Observable<RuleType> getObservableRuleType() {
        return mRuleType;
    }

    public RuleType getRuleType() {
        return mRuleType.getValue();
    }

    public void setRuleType(RuleType ruleType) {
        mRuleType.onNext(ruleType);
    }

    public Observable<RuleSatisfyType> getObservableRuleSatisfyType() {
        return mRuleSatisfyType;
    }

    public RuleSatisfyType getRuleSatisfyType() {
        return mRuleSatisfyType.getValue();
    }

    public void setRuleSatisfyType(RuleSatisfyType ruleSatisfyType) {
        mRuleSatisfyType.onNext(ruleSatisfyType);
    }

    public Observable<RuleLimitation> getObservableRuleLimitation() {
        return mRuleLimitation;
    }

    public RuleLimitation getRuleLimitation() {
        return mRuleLimitation.getValue();
    }

    public void setRuleLimitation(RuleLimitation ruleLimitation) {
        mRuleLimitation.onNext(ruleLimitation);
    }

    public Observable<Date> getObservableLimitationTimeFrom() {
        return mLimitationTimeFrom;
    }

    public Date getLimitationTimeFrom() {
        return mLimitationTimeFrom.getValue();
    }

    public void setLimitationTimeFrom(Date limitationTimeFrom) {
        if (limitationTimeFrom.compareTo(mLimitationTimeUntil.getValue()) > 0) {
            Calendar calendar = Calendar.getInstance();
            calendar.setTime(limitationTimeFrom);
            calendar.add(Calendar.HOUR, 1);
            mLimitationTimeUntil.onNext(calendar.getTime());
        }
        mLimitationTimeFrom.onNext(limitationTimeFrom);
    }

    public Observable<Date> getObservableLimitationTimeUntil() {
        return mLimitationTimeUntil;
    }

    public Date getLimitationTimeUntil() {
        return mLimitationTimeUntil.getValue();
    }

    public void setLimitationTimeUntil(Date limitationTimeUntil) {
        if (mLimitationTimeFrom.getValue().compareTo(limitationTimeUntil) > 0) {
            Calendar calendar = Calendar.getInstance();
            calendar.setTime(limitationTimeUntil);
            calendar.add(Calendar.HOUR, -1);
            mLimitationTimeFrom.onNext(calendar.getTime());
        }
        mLimitationTimeUntil.onNext(limitationTimeUntil);
    }

    public LocationRule.Unit getLocationUnit() {
        return mLocationUnit;
    }

    public SpeedLimitRule.Unit getSpeedLimitUnit() {
        return mSpeedLimitUnit;
    }

    public void setSpeedLimitUnit(SpeedLimitRule.Unit speedLimitUnit) {
        mSpeedLimitUnit = speedLimitUnit;
    }

    public void setLocationUnit(LocationRule.Unit locationUnit) {
        mLocationUnit = locationUnit;
    }

    public Rule makeNewRule() {
        Rule rule = null;
        switch (mRuleType.getValue()) {
            case SINGLE:
                switch (mRuleLimitation.getValue()) {
                    case TIME:
                        rule =  new TimeRule(mLimitationTimeFrom.getValue(), mLimitationTimeUntil.getValue());
                        break;
                    case LOCATION:
                        rule = validateAndCreateLocationRule();
                        break;
                    case SPEED_LIMIT:
                        rule = validateAndCreateSpeedLimitRule();
                        break;
                }
                break;
            case MULTIPLE:
                rule = validateAndCreateMultipleRule();
                break;
        }
        if (rule != null) {
            if (mRule != null) {
                UUID uuid = mRule.getId();
                rule.setId(uuid);
                mRule = rule;
            }
        }
        return rule;
    }

    @Nullable
    private LocationRule validateAndCreateLocationRule() {
        String message = null;

        if (mRadius == null || mRadius.isEmpty())
            message = mResourceProvider.getString(R.string.msg_empty_radius);
        if (mLongitude == null || mLatitude.isEmpty())
            message = mResourceProvider.getString(R.string.msg_empty_longitude);
        if (mLatitude == null || mLatitude.isEmpty())
            message = mResourceProvider.getString(R.string.msg_empty_latitude);

        if (message != null) {
            mShowMessage.onNext(message);
            return null;
        }
        return new LocationRule(
                Float.valueOf(mLatitude),
                Float.valueOf(mLongitude),
                Float.valueOf(mRadius),
                mLocationUnit);
    }

    @Nullable
    private SpeedLimitRule validateAndCreateSpeedLimitRule() {
        String message = null;
        if (mSpeedLimit == null || TextUtils.isEmpty(mSpeedLimit))
            message = mResourceProvider.getString(R.string.msg_empty_speed_limit);

        if (message != null) {
            mShowMessage.onNext(message);
            return null;
        }

        return new SpeedLimitRule(
                Float.valueOf(mSpeedLimit),
                mSpeedLimitUnit);
    }

    @Nullable
    private MultipleRule validateAndCreateMultipleRule() {
        String message = null;

        if (mRuleList.getValue().isEmpty())
            message = mResourceProvider.getString(R.string.error_msg_no_rules_added);
        if (message != null) {
            mShowMessage.onNext(message);
            return null;
        }
        return new MultipleRule(mRuleList.getValue(), mRuleSatisfyType.getValue());
    }

}
