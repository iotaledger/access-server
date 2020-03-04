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

package com.de.xain.emdac.login;

import android.app.Activity;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.authenteq.android.flow.ui.seflieauthentication.SelfieAuthenticationActivity;
import com.de.xain.emdac.BaseActivity;
import com.de.xain.emdac.CommunicationFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.SettingsFragment;
import com.de.xain.emdac.databinding.FragmentLoginBinding;
import com.de.xain.emdac.models.User;
import com.de.xain.emdac.navigation.NavigationDrawerActivity;
import com.de.xain.emdac.register.RegisterFragment;
import com.de.xain.emdac.user.UserManager;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import timber.log.Timber;

import static com.de.xain.emdac.utils.Constants.AUTHENTEQ_LICENSE_KEY;

public class LoginFragment extends CommunicationFragment<LoginViewModel> implements
        View.OnClickListener {

    @Inject
    public UserManager mUserManager;
    private FragmentLoginBinding mBinding;

    public static LoginFragment newInstance() {
        return new LoginFragment();
    }

    @Override
    protected Class<LoginViewModel> getViewModelClass() {
        return LoginViewModel.class;
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_login, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mBinding.setViewModel(mViewModel);
        mBinding.buttonConnect.setOnClickListener(this);
        mBinding.buttonCreateAccount.setOnClickListener(this);
        setHasOptionsMenu(true);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.menu_login, menu);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.action_settings) {
            Activity activity = getActivity();
            if (activity instanceof BaseActivity) {
                ((BaseActivity) activity).addFragmentToBackStack(SettingsFragment.newInstance(false), LoginActivity.FRAGMENT_TAG_SETTINGS);
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
                .getLoginCompleted()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::onLoginComplete,
                        Timber::e));

        mDisposable.add(mViewModel
                .getObservableAuthenteqUserId()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::startSelfieAuthentication,
                        Timber::e));
    }

    public void onAuthenteqLoginResult(boolean isSuccessful) {
        if (isSuccessful)
            mViewModel.getUser();
        else
            showDialogMessage(getString(R.string.msg_unable_to_log_in));
    }

    private void onLoginComplete(User user) {
        Activity activity = getActivity();
        if (activity == null) return;
        mUserManager.startSession(user);
        startActivity(NavigationDrawerActivity.newIntent(activity));
        Timber.d("Successfully logged in");
        activity.finish();
    }

    public void startSelfieAuthentication(String userId) {
        Activity activity = getActivity();
        if (activity == null) return;

        SelfieAuthenticationActivity.startForResult(
                activity,
                LoginActivity.AUTHENTEQ_LOGIN_REQUEST_CODE,
                AUTHENTEQ_LICENSE_KEY,
                userId);
    }

    @Override
    public void onClick(View v) {
        if (v.equals(mBinding.buttonCreateAccount)) {
            if (getActivity() instanceof BaseActivity) {
                ((BaseActivity) getActivity()).addFragmentToBackStack(RegisterFragment.newInstance(), LoginActivity.FRAGMENT_TAG_REGISTER);
            }
        } else if (v.equals(mBinding.buttonConnect)) {
            mViewModel.getAuthenteqUserId();
        }
    }
}
