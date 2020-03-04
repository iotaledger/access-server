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

import android.app.Activity;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.arch.lifecycle.ViewModelProvider;
import android.arch.lifecycle.ViewModelProviders;
import android.databinding.DataBindingUtil;
import android.databinding.ViewDataBinding;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;

import com.de.xain.emdac.BaseActivity;
import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.FragmentDelegationRuleBinding;
import com.de.xain.emdac.databinding.ViewMakeRuleLocationBinding;
import com.de.xain.emdac.databinding.ViewMakeRuleSpeedLimitBinding;
import com.de.xain.emdac.databinding.ViewMakeRuleTimeBinding;
import com.de.xain.emdac.delegation.LocationRule;
import com.de.xain.emdac.delegation.Rule;
import com.de.xain.emdac.delegation.RuleInformable;
import com.de.xain.emdac.delegation.RuleLimitation;
import com.de.xain.emdac.delegation.RuleSatisfyType;
import com.de.xain.emdac.delegation.RuleType;
import com.de.xain.emdac.delegation.RulesAdapter;
import com.de.xain.emdac.delegation.SpeedLimitRule;
import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.utils.Constants;
import com.de.xain.emdac.utils.ui.SpinnerArrayAdapter;
import com.de.xain.emdac.utils.ui.ViewLifecycleFragment;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import timber.log.Timber;

public class DelegationRuleFragment extends ViewLifecycleFragment implements
        Injectable,
        RulesAdapter.OnItemClickListener,
        RuleInformable {

    private static final String ENABLE_MULTIPLE = "com.de.xain.emdac.delegation.enable_multiple";
    private static final String CALLER_TAG = "com.de.xain.emdac.delegation.caller_tag";
    private static final String RULE = "com.de.xain.emdac.delegation.mRule";

    /**
     * Due to nested calls, each delegation mRule fragment tag should be unique,
     * so timestamp is added at end.
     */
    private final String DELEGATION_RULE_FRAGMENT_TAG = "com.de.xain.emdac.delegation.delegation_rule_fragment" + String.valueOf(new Date().getTime());

    @Inject
    public ViewModelProvider.Factory mViewModelFactory;

    private FragmentDelegationRuleBinding mBinding;
    private DelegationRuleViewModel mViewModel;
    private CompositeDisposable mDisposable;
    private RulesAdapter mRulesAdapter;
    private List<Rule> mRuleList = new ArrayList<>();
    private String mCallerTag;
    private InternalRuleDelegator mRuleDelegator;
    private boolean mEnableMultiple;

    @Nullable
    private Rule mRule;

    /**
     * @param callerTag      Tag of fragment that created this one
     * @param rule           Rule to fill UI
     * @param enableMultiple Flag to enable switching between single and multiple rule type
     * @return New instance of this class
     */
    public static DelegationRuleFragment newInstance(@NonNull String callerTag, @Nullable Rule rule, boolean enableMultiple) {
        Bundle args = new Bundle();
        args.putString(CALLER_TAG, callerTag);
        args.putBoolean(ENABLE_MULTIPLE, enableMultiple);
        if (rule != null)
            args.putParcelable(RULE, rule);
        DelegationRuleFragment fragment = new DelegationRuleFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        extractArgs();
    }

    private void extractArgs() {
        Bundle args = getArguments();
        if (args == null) return;

        if (args.containsKey(CALLER_TAG))
            mCallerTag = args.getString(CALLER_TAG);
        else
            Timber.e("No caller tag!!!");

        if (args.containsKey(ENABLE_MULTIPLE))
            mEnableMultiple = args.getBoolean(ENABLE_MULTIPLE);
        else
            mEnableMultiple = false;

        if (args.containsKey(RULE))
            mRule = args.getParcelable(RULE);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_delegation_rule, container, false);

        ArrayAdapter<CharSequence> adapter;

        // type of mRule
        adapter = new SpinnerArrayAdapter<>(getActivity(), new String[]{
                getString(RuleType.SINGLE.nameResId()),
                getString(RuleType.MULTIPLE.nameResId())
        });
        mBinding.spinnerTypeOfRule.setAdapter(adapter);

        mBinding.labelTypeOfRule.setVisibility(View.GONE);
        mBinding.spinnerTypeOfRule.setVisibility(View.GONE);

        // mRule satisfy type
        adapter = new SpinnerArrayAdapter<>(getActivity(), new String[]{
                getString(RuleSatisfyType.ONE_OR_MORE.getNameResId()),
                getString(RuleSatisfyType.EVERY.getNameResId())
        });
        mBinding.spinnerSatisfyType.setAdapter(adapter);

        // mRule limit by
        adapter = new SpinnerArrayAdapter<>(getActivity(), RuleLimitation.getAllStringValues(getResources()));
        mBinding.spinnerLimitBy.setAdapter(adapter);

        mBinding.buttonDelegate.setOnClickListener(__ -> onRuleMake());

        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mRuleDelegator = (InternalRuleDelegator) getActivity();
        mViewModel = ViewModelProviders.of(this, mViewModelFactory).get(DelegationRuleViewModel.class);
        if (mRule != null)
            mViewModel.setInitialRule(mRule);
        mBinding.setViewModel(mViewModel);

        if (mRule != null) {
            setHasOptionsMenu(true);
            mBinding.buttonDelegate.setText(R.string.button_save);
        }

        mViewModel.setRuleType(mEnableMultiple ? RuleType.MULTIPLE : RuleType.SINGLE);
    }

    @Override
    public void onResume() {
        super.onResume();
        bindViewModel();
    }

    @Override
    public void onStop() {
        unbindViewModel();
        super.onStop();
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_delegation_rule, menu);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_delete:
                showDialogMessage(
                        getString(R.string.msg_delete_rule_question),
                        __ -> {
                            mRuleDelegator.delegateRuleDeleted(mViewModel.getRule(), mCallerTag);
                            finish();
                        }
                );
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void bindViewModel() {
        mDisposable = new CompositeDisposable();

        // rules list
        mDisposable.add(mViewModel.getObservableRuleList()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleListChange,
                        // on error
                        Timber::e));

        // dialog messages
        mDisposable.add(mViewModel.getShowMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::showDialogMessage,
                        // on error
                        Timber::e));

        // mRule type
        mDisposable.add(mViewModel.getObservableRuleType()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleTypeChange,
                        // on error
                        Timber::e));

        // mRule satisfy type
        mDisposable.add(mViewModel.getObservableRuleSatisfyType()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleSatisfyTypeChange,
                        // on error
                        Timber::e));

        // mRule limitation
        mDisposable.add(mViewModel.getObservableRuleLimitation()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleLimitationChange,
                        // on error
                        Timber::e));

        // mRule limitation time from
        mDisposable.add(mViewModel.getObservableLimitationTimeFrom()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleLimitationTimeFromChange,
                        // on error
                        Timber::e));

        // mRule limitation time until
        mDisposable.add(mViewModel.getObservableLimitationTimeUntil()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        // on next
                        this::onRuleLimitationTimeUntilChange,
                        // on error
                        Timber::e));
    }

    private void unbindViewModel() {
        mDisposable.dispose();
    }

    private void onRuleListChange(List<Rule> ruleList) {
        mRuleList.clear();
        mRuleList.addAll(ruleList);
        mRulesAdapter.notifyDataSetChanged();
    }

    private void onRuleTypeChange(RuleType ruleType) {
        switch (ruleType) {
            case SINGLE:
                mBinding.spinnerSatisfyType.setVisibility(View.GONE);
                mBinding.labelSatisfyType.setVisibility(View.GONE);

                mBinding.labelLimitBy.setVisibility(View.VISIBLE);
                mBinding.spinnerLimitBy.setVisibility(View.VISIBLE);

                onRuleLimitationChange(mViewModel.getRuleLimitation());
                break;
            case MULTIPLE:
                mBinding.spinnerSatisfyType.setVisibility(View.VISIBLE);
                mBinding.labelSatisfyType.setVisibility(View.VISIBLE);

                mBinding.labelLimitBy.setVisibility(View.GONE);
                mBinding.spinnerLimitBy.setVisibility(View.GONE);

                addRulesRecyclerView();
                break;
        }
    }

    private void onRuleSatisfyTypeChange(RuleSatisfyType ruleSatisfyType) {
        switch (ruleSatisfyType) {
            case ONE_OR_MORE:
                break;
            case EVERY:
                break;
        }
    }

    private void onRuleLimitationChange(RuleLimitation ruleLimitation) {
        if (mViewModel.getRuleType() != RuleType.SINGLE) return;

        mBinding.frameLayoutDefineRule.removeAllViews();

        switch (ruleLimitation) {
            case TIME: {
                ViewMakeRuleTimeBinding timeBinding = DataBindingUtil.inflate(LayoutInflater.from(getActivity()), R.layout.view_make_rule_time, null, false);
                mBinding.frameLayoutDefineRule.addView(timeBinding.getRoot());
                timeBinding.buttonFrom.setOnClickListener(__ -> openDatePicker(RuleLimitation.TimeType.FROM));
                timeBinding.buttonUntil.setOnClickListener(__ -> openDatePicker(RuleLimitation.TimeType.UNTIL));
                onRuleLimitationTimeFromChange(mViewModel.getLimitationTimeFrom());
                onRuleLimitationTimeUntilChange(mViewModel.getLimitationTimeUntil());
                break;
            }
            case LOCATION: {
                ViewMakeRuleLocationBinding locationBinding = DataBindingUtil.inflate(LayoutInflater.from(getActivity()), R.layout.view_make_rule_location, null, false);
                mBinding.frameLayoutDefineRule.addView(locationBinding.getRoot());
                locationBinding.setViewModel(mViewModel);
                ArrayAdapter<LocationRule.Unit> adapter = new SpinnerArrayAdapter<>(getActivity(), LocationRule.Unit.allValues());
                locationBinding.spinnerUnit.setAdapter(adapter);
                locationBinding.spinnerUnit.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                    @Override
                    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                        mViewModel.setLocationUnit((LocationRule.Unit) parent.getItemAtPosition(position));
                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> parent) {
                    }
                });
                locationBinding.spinnerUnit.setSelection(mViewModel.getLocationUnit().ordinal());

                break;
            }
            case SPEED_LIMIT:
                ViewMakeRuleSpeedLimitBinding speedLimitBinding = DataBindingUtil.inflate(LayoutInflater.from(getActivity()), R.layout.view_make_rule_speed_limit, null, false);
                mBinding.frameLayoutDefineRule.addView(speedLimitBinding.getRoot());
                speedLimitBinding.setViewModel(mViewModel);
                ArrayAdapter<SpeedLimitRule.Unit> adapter = new SpinnerArrayAdapter<>(getActivity(), SpeedLimitRule.Unit.allValues());
                speedLimitBinding.spinnerUnit.setAdapter(adapter);
                speedLimitBinding.spinnerUnit.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                    @Override
                    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                        mViewModel.setSpeedLimitUnit((SpeedLimitRule.Unit) parent.getItemAtPosition(position));
                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> parent) {
                    }
                });
                speedLimitBinding.spinnerUnit.setSelection(mViewModel.getSpeedLimitUnit().ordinal());

                break;
        }
    }

    private void openDatePicker(RuleLimitation.TimeType timeType) {
        if (getActivity() == null) return;

        final Calendar calendar = Calendar.getInstance();
        switch (timeType) {
            case FROM:
                calendar.setTime(mViewModel.getLimitationTimeFrom());
                break;
            case UNTIL:
                calendar.setTime(mViewModel.getLimitationTimeUntil());
                break;
        }

        DatePickerDialog dialog = new DatePickerDialog(
                getActivity(),
                (view, year, month, dayOfMonth) -> openTimePicker(timeType, calendar, year, month, dayOfMonth),
                calendar.get(Calendar.YEAR),
                calendar.get(Calendar.MONTH),
                calendar.get(Calendar.DATE));

        dialog.setOnShowListener(dialog1 -> {
            if (dialog1 instanceof DatePickerDialog) {
                DatePickerDialog datePickerDialog = ((DatePickerDialog) dialog1);
                Button positiveButton = datePickerDialog.getButton(DatePickerDialog.BUTTON_POSITIVE);
                if (positiveButton != null) {
                    positiveButton.setTextColor(Color.BLACK);
                }
                Button negativeButton = datePickerDialog.getButton(DatePickerDialog.BUTTON_NEGATIVE);
                if (negativeButton != null) {
                    negativeButton.setTextColor(Color.BLACK);
                }
            }
        });

        dialog.show();
    }

    private void openTimePicker(RuleLimitation.TimeType timeType, final Calendar calendar, int year, int month, int dayOfMonth) {
        TimePickerDialog dialog = new TimePickerDialog(
                getActivity(),
                (view1, hourOfDay, minute) -> {
                    Calendar date = Calendar.getInstance();
                    date.set(year, month, dayOfMonth, hourOfDay, minute);
                    switch (timeType) {
                        case FROM:
                            mViewModel.setLimitationTimeFrom(date.getTime());
                            break;
                        case UNTIL:
                            mViewModel.setLimitationTimeUntil(date.getTime());
                            break;
                    }
                },
                calendar.get(Calendar.HOUR_OF_DAY),
                calendar.get(Calendar.MINUTE),
                true);

        dialog.setOnShowListener(dialog1 -> {
            if (dialog1 instanceof TimePickerDialog) {
                TimePickerDialog timePickerDialog = ((TimePickerDialog) dialog1);
                Button positiveButton = timePickerDialog.getButton(TimePickerDialog.BUTTON_POSITIVE);
                if (positiveButton != null) {
                    positiveButton.setTextColor(Color.BLACK);
                }
                Button negativeButton = timePickerDialog.getButton(TimePickerDialog.BUTTON_NEGATIVE);
                if (negativeButton != null) {
                    negativeButton.setTextColor(Color.BLACK);
                }
            }
        });
        dialog.show();
    }

    private void onRuleLimitationTimeFromChange(Date date) {
        ViewMakeRuleTimeBinding timeBinding = getRuleTimeBinding();
        if (timeBinding == null) return;
        SimpleDateFormat format = new SimpleDateFormat(Constants.DATE_AND_TIME_FORMAT, Locale.getDefault());
        timeBinding.buttonFrom.setText(format.format(date));
    }

    private void onRuleLimitationTimeUntilChange(Date date) {
        ViewMakeRuleTimeBinding timeBinding = getRuleTimeBinding();
        if (timeBinding == null) return;
        SimpleDateFormat format = new SimpleDateFormat(Constants.DATE_AND_TIME_FORMAT, Locale.getDefault());
        timeBinding.buttonUntil.setText(format.format(date));
    }

    @Nullable
    private ViewMakeRuleTimeBinding getRuleTimeBinding() {
        if (mBinding.frameLayoutDefineRule.getChildCount() < 1) return null;
        ViewDataBinding binding = DataBindingUtil.getBinding(mBinding.frameLayoutDefineRule.getChildAt(0));
        if (binding instanceof ViewMakeRuleTimeBinding)
            return (ViewMakeRuleTimeBinding) binding;
        return null;
    }

    private void addRulesRecyclerView() {
        mBinding.frameLayoutDefineRule.removeAllViews();

        RecyclerView recyclerView = LayoutInflater.from(getActivity())
                .inflate(R.layout.recycler_view, mBinding.frameLayoutDefineRule, false)
                .findViewById(R.id.recycler_view);
        recyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));

        mRulesAdapter = new RulesAdapter(mRuleList, this);
        recyclerView.setAdapter(mRulesAdapter);

        mBinding.frameLayoutDefineRule.addView(recyclerView);
    }

    @Override
    public void onAddRuleItemClick() {
        Activity activity = getActivity();
        if (activity instanceof BaseActivity) {
            ((BaseActivity) activity).addFragmentToBackStack(DelegationRuleFragment.newInstance(getTag(), null, false), DELEGATION_RULE_FRAGMENT_TAG);
        }
    }

    @Override
    public void onRuleSelect(Rule rule, RuleType ruleType) {
        Activity activity = getActivity();
        if (activity instanceof BaseActivity) {
            ((BaseActivity) activity).addFragmentToBackStack(DelegationRuleFragment.newInstance(getTag(), rule, ruleType == RuleType.MULTIPLE), DELEGATION_RULE_FRAGMENT_TAG);
        }
    }

    @Override
    public void onRuleDelete(Rule rule) {
        mViewModel.removeRule(rule);
    }

    /**
     * Called when user taps "Make" button.
     */
    private void onRuleMake() {
        Rule rule = mViewModel.makeNewRule();
        if (rule != null) {
            if (!mViewModel.isEditingExistingRule())
                mRuleDelegator.delegateNewRule(rule, mCallerTag);
            else
                mRuleDelegator.delegateEditedRule(rule, mCallerTag);
            finish();
        }
    }

    @Override
    public void onRuleEdited(Rule rule) {
        if (mRule != null) {
            UUID id = mRule.getId();
            mRule = rule;
            mRule.setId(id);
        }
    }

    @Override
    public void onNewRuleCreated(Rule rule) {
        mViewModel.addRule(rule);
    }

    public interface InternalRuleDelegator {
        /**
         * Called when new policy attribute is made, so it can be delegated to another fragment.
         *
         * @param rule New policy attribute
         * @param tag  Fragment's tag which asked for making new mRule
         */
        void delegateNewRule(Rule rule, String tag);

        void delegateEditedRule(Rule rule, String tag);

        void delegateRuleDeleted(Rule rule, String tag);
    }
}
