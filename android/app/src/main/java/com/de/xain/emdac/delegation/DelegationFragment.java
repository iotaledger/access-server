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

import android.app.Activity;
import android.databinding.DataBindingUtil;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.LinearLayoutManager;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.BaseActivity;
import com.de.xain.emdac.CommunicationFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.FragmentDelegationBinding;
import com.de.xain.emdac.delegation.preview.DelegationPreviewFragment;
import com.de.xain.emdac.delegation.rule.DelegationRuleFragment;
import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.utils.Optional;
import com.de.xain.emdac.utils.ui.DialogFragmentUtil;
import com.de.xain.emdac.utils.ui.SpinnerArrayAdapter;
import com.de.xain.emdac.utils.ui.ThemeLab;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import timber.log.Timber;

import static com.de.xain.emdac.utils.Constants.TOKEN_SCALE_FACTOR;

public class DelegationFragment extends CommunicationFragment<DelegationViewModel> implements
        View.OnClickListener,
        RuleInformable,
        RulesAdapter.OnItemClickListener {

    private static final int MENU_ITEM_PREVIEW = 1;
    private static final String DELEGATION_PREVIEW_FRAGMENT_TAG = "com.de.xain.emdac.delegation.preview_fragment";
    private static final String DELEGATION_RULE_FRAGMENT_TAG = "com.de.xain.emdac.delegationeditor2.delegation_rule_fragment";
    private static final String USERS_LIST_FRAGMENT_TAG = "com.de.xain.emdac.delegationeditor2.users_list_fragment";

    @Inject
    ThemeLab mThemeLab;
    @Inject
    AppSharedPreferences mPreferences;

    private FragmentDelegationBinding mBinding;
    private RulesAdapter mRulesAdapter;
    private List<Rule> mRuleList = new ArrayList<>();

    public static DelegationFragment newInstance() {
        return new DelegationFragment();
    }

    @Override
    protected Class<DelegationViewModel> getViewModelClass() {
        return DelegationViewModel.class;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mViewModel.getAllUsers().getValue().isEmpty()) {
            mViewModel.requestAllUsers();
        } else {
            mBinding.constraintLayout.setVisibility(View.VISIBLE);
        }
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_delegation, container, false);

        mBinding.buttonUsers.setOnClickListener(this);
        mBinding.buttonActions.setOnClickListener(this);

        mBinding.buttonOneTimeOnly.setOnClickListener(this);
        mBinding.buttonUnlimited.setOnClickListener(this);

        String[] values = new String[DelegationViewModel.COST_VALUES.length];
        for (int i = 0; i < DelegationViewModel.COST_VALUES.length; ++i) {
            int quantity = (int) (DelegationViewModel.COST_VALUES[i] * TOKEN_SCALE_FACTOR);
            if (quantity == 0) values[i] = getResources().getString(R.string.free);
            else
                values[i] = getResources().getQuantityString(R.plurals.token_plural, quantity, String.valueOf((int) (DelegationViewModel.COST_VALUES[i] * TOKEN_SCALE_FACTOR)));
        }
        mBinding.spinnerCost.setAdapter(new SpinnerArrayAdapter<>(getActivity(), values));

        mBinding.buttonDelegate.setOnClickListener(this);

        mRulesAdapter = new RulesAdapter(mRuleList, this);
        mBinding.rulesRecyclerView.setAdapter(mRulesAdapter);
        mBinding.rulesRecyclerView.setLayoutManager(new LinearLayoutManager(getActivity()));

        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mBinding.setViewModel(mViewModel);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        MenuItem menuItem = menu.add(Menu.NONE, MENU_ITEM_PREVIEW, 0, R.string.action_preview);
        if (getContext() != null) {
            Drawable icon = ContextCompat.getDrawable(getContext(), R.drawable.ic_preview);
            if (icon != null)
                menuItem.setIcon(icon);
        }
        menuItem.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM | MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == MENU_ITEM_PREVIEW) {
            if (getActivity() instanceof BaseActivity) {
                ((BaseActivity) getActivity()).addFragmentToBackStack(DelegationPreviewFragment.newInstance(mViewModel.createPolicy(true, null)), DELEGATION_PREVIEW_FRAGMENT_TAG);
            }
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    @Override
    protected void bindViewModel() {
        super.bindViewModel();

        mDisposable.add(mViewModel
                .getAllUsers()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onGetAllUsers,
                        Timber::e));

        // rules list change
        mDisposable.add(mViewModel
                .getObservableRuleList()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onRuleListChange,
                        Timber::e));

        // user list change
        mDisposable.add(mViewModel
                .getObservableSelectedUsers()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onUserListChange,
                        Timber::e));

        // action list change
        mDisposable.add(mViewModel
                .getObservableDelegationActionList()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onDelegationActionListChange,
                        Timber::e));

        mDisposable.add(mViewModel
                .getObservableMaxNumOfExecutions()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onMaximumExecutionNumberChange,
                        Timber::e));
    }

    @Override
    public void onClick(View v) {
        if (v.equals(mBinding.buttonUsers)) {
            if (getActivity() instanceof BaseActivity) {
                if (mViewModel.getAllUsers().getValue().isEmpty()) return;

                Set<UserPublic> selectedUsers = mViewModel.getSelectedUsers();

                List<UserPublic> allUsers = mViewModel.getAllUsers().getValue().get();


                boolean[] selected = new boolean[allUsers.size()];
                for (int i = 0; i < selected.length; ++i)
                    selected[i] = false;

                for (UserPublic selectedUser : selectedUsers) {
                    int index = allUsers.indexOf(selectedUser);
                    if (index >= 0) {
                        selected[index] = true;
                    }
                }

                CharSequence[] userNames = new CharSequence[allUsers.size()];
                for (int i = 0, n = allUsers.size(); i < n; ++i)
                    userNames[i] = allUsers.get(i).getUsername();

                DialogFragmentUtil.showDialog(
                        DialogFragmentUtil.createListDialog(
                                userNames,
                                selected,
                                this::onSelectionUsersFinish),
                        getChildFragmentManager(),
                        USERS_LIST_FRAGMENT_TAG);
                return;
            }
        }

        if (v.equals(mBinding.buttonActions)) {
            List<DelegationAction> delegationActionList = mViewModel.getDelegationActionList();

            List<CharSequence> delegationActionNames = new ArrayList<>();
            for (int i = 0, n = mViewModel.ALL_ACTIONS.length; i < n; ++i)
                delegationActionNames.add(mViewModel.ALL_ACTIONS[i].getName());

            List<DelegationAction> allActions = Arrays.asList(mViewModel.ALL_ACTIONS);

            boolean[] selectedActions = new boolean[allActions.size()];
            for (int i = 0; i < selectedActions.length; ++i)
                selectedActions[i] = false;

            for (int i = 0, n = delegationActionList.size(); i < n; ++i) {
                int index = allActions.indexOf(delegationActionList.get(i));
                if (index >= 0)
                    selectedActions[index] = true;
            }

            DialogFragmentUtil.showDialog(
                    DialogFragmentUtil.createListDialog(
                            delegationActionNames.toArray(new CharSequence[0]),
                            selectedActions,
                            this::onSelectionActionsFinish),
                    getChildFragmentManager(),
                    USERS_LIST_FRAGMENT_TAG);
            return;
        }

        if (v.equals(mBinding.buttonDelegate)) {
            mViewModel.delegate();
            return;
        }

        if (v.equals(mBinding.buttonOneTimeOnly)) {
            mViewModel.setMaxNumOfExecutions(1);
            return;
        }

        if (v.equals(mBinding.buttonUnlimited)) {
            mViewModel.setMaxNumOfExecutions(null);
            return;
        }
    }

    @Override
    public void onRuleEdited(Rule rule) {
        mViewModel.updateRule(rule);
    }

    @Override
    public void onNewRuleCreated(Rule rule) {
        mViewModel.addRule(rule);
    }

    @Override
    public void onRuleDelete(Rule rule) {
        mViewModel.removeRule(rule);
    }

    private void onUserListChange(Set<UserPublic> userPublicList) {
        String label = getString(R.string.label_delegate_to) + " (" + getString(R.string.label_selected, userPublicList.size()) + ")";
        mBinding.labelDelegateTo.setText(label);
    }

    private void onDelegationActionListChange(List<DelegationAction> delegationActionList) {
        String label = getString(R.string.label_actions) + " (" + getString(R.string.label_selected, delegationActionList.size()) + ")";
        mBinding.labelActions.setText(label);
    }

    private void onSelectionUsersFinish(boolean[] selected) {
        Set<UserPublic> users = new HashSet<>();

        for (int i = 0; i < selected.length; ++i)
            if (selected[i]) users.add(mViewModel.getAllUsers().getValue().get().get(i));

        mViewModel.setSelectedUsers(users);
    }

    private void onSelectionActionsFinish(boolean[] selected) {
        List<DelegationAction> actions = new ArrayList<>();

        for (int i = 0; i < selected.length; ++i)
            if (selected[i]) actions.add(mViewModel.ALL_ACTIONS[i]);

        mViewModel.setDelegationList(actions);
    }

    private void onMaximumExecutionNumberChange(Optional<Integer> integerOptional) {
        if (integerOptional.isEmpty()) {
            mBinding.buttonUnlimited.setChecked(true);
        } else {
            mBinding.buttonOneTimeOnly.setChecked(true);
        }
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

    private void onRuleListChange(List<Rule> ruleList) {
        mRuleList.clear();
        mRuleList.addAll(ruleList);
        mRulesAdapter.notifyDataSetChanged();
    }

    private void onGetAllUsers(Optional<List<UserPublic>> allUsers) {
        if (allUsers.isEmpty()) {
            mBinding.constraintLayout.setVisibility(View.INVISIBLE);
        } else {
            mBinding.constraintLayout.setVisibility(View.VISIBLE);
        }
    }
}
