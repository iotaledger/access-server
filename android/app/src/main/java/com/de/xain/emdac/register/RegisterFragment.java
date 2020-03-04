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

package com.de.xain.emdac.register;

import android.app.Activity;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.authenteq.android.flow.ui.onboarding.OnboardingActivity;
import com.de.xain.emdac.CommunicationFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.FragmentRegisterBinding;
import com.de.xain.emdac.login.LoginActivity;
import com.de.xain.emdac.models.User;
import com.de.xain.emdac.navigation.NavigationDrawerActivity;
import com.de.xain.emdac.user.UserManager;
import com.de.xain.emdac.utils.Constants;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import timber.log.Timber;

public class RegisterFragment extends CommunicationFragment<RegisterViewModel> implements
        View.OnClickListener {

    @Inject
    public UserManager mUserManager;

    private FragmentRegisterBinding mBinding;

    public static RegisterFragment newInstance() {
        return new RegisterFragment();
    }

    @Override
    protected Class<RegisterViewModel> getViewModelClass() {
        return RegisterViewModel.class;
    }

    public void onOnboardingResultReceive(OnboardingActivity.Result result) {
        mViewModel.setStage(RegisterViewModel.Stage.SECOND);
        mViewModel.setFirstName(result.getDocument().getGivenName());
        mViewModel.setLastName(result.getDocument().getLastName());
        mViewModel.setUserId(result.getUserId());
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_register, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mBinding.setViewModel(mViewModel);
        mBinding.buttonConnect.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v.equals(mBinding.buttonConnect)) {
            if (mViewModel.getStage() == RegisterViewModel.Stage.FIRST) {
                mViewModel.checkUsername();
            } else {
                mViewModel.registerUser();
            }
        }
    }

    public void startOnboarding(boolean start) {
        if (start) {
            Activity activity = getActivity();
            if (activity == null) return;

            OnboardingActivity.startForResult(
                    activity,
                    LoginActivity.AUTHENTEQ_CREATE_ACCOUNT_REQUEST_CODE,
                    Constants.AUTHENTEQ_LICENSE_KEY);
        }
    }

    @Override
    protected void bindViewModel() {
        super.bindViewModel();

        mDisposable.add(mViewModel
                .getObservableStartOnboarding()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::startOnboarding,
                        Timber::e));

        mDisposable.add(mViewModel
                .getRegisterCompleted()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onRegisterComplete,
                        Timber::e));
    }

    private void onRegisterComplete(User user) {
        Activity activity = getActivity();
        if (activity == null) return;
        mUserManager.startSession(user);
        startActivity(NavigationDrawerActivity.newIntent(activity));
        Timber.d("Successfully logged in");
        activity.finish();
    }

}
