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

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;

import com.authenteq.android.flow.ui.onboarding.OnboardingActivity;
import com.authenteq.android.flow.ui.seflieauthentication.SelfieAuthenticationActivity;
import com.de.xain.emdac.BaseActivity;
import com.de.xain.emdac.register.RegisterFragment;

import dagger.android.AndroidInjection;
import timber.log.Timber;

/**
 * Activity representing login screen
 */
public class LoginActivity extends BaseActivity {

    public static final int AUTHENTEQ_CREATE_ACCOUNT_REQUEST_CODE = 100;
    public static final int AUTHENTEQ_LOGIN_REQUEST_CODE = 101;
    public static final String FRAGMENT_TAG_REGISTER = "com.de.xain.emdac.fragment_register";
    public static final String FRAGMENT_TAG_SETTINGS = "com.de.xain.emdac.fragment_settings";
    private static final String FRAGMENT_TAG_LOGIN = "com.de.xain.emdac.fragment_login";

    public static Intent newIntent(Context context) {
        return new Intent(context, LoginActivity.class);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        AndroidInjection.inject(this);
        super.onCreate(savedInstanceState);

        Fragment fragment = getCurrentFragment();
        if (fragment == null) {
            fragment = LoginFragment.newInstance();
            addFragmentToBackStack(fragment, FRAGMENT_TAG_LOGIN);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        switch (requestCode) {
            case AUTHENTEQ_CREATE_ACCOUNT_REQUEST_CODE:
                if (resultCode == RESULT_OK) {
                    if (data == null) return;
                    final OnboardingActivity.Result result = OnboardingActivity.getResult(data);
                    FragmentManager fm = getSupportFragmentManager();
                    Fragment fragment = fm.findFragmentByTag(FRAGMENT_TAG_REGISTER);
                    if (fragment instanceof RegisterFragment)
                        ((RegisterFragment) fragment).onOnboardingResultReceive(result);
                } else {
                    Timber.d("Authenteq user creation canceled");
                }
                break;
            case AUTHENTEQ_LOGIN_REQUEST_CODE:
                if (resultCode == RESULT_OK) {
                    if (data == null) return;
                    final SelfieAuthenticationActivity.Result result = SelfieAuthenticationActivity.getResult(data);
                    FragmentManager fm = getSupportFragmentManager();
                    Fragment fragment = fm.findFragmentByTag(FRAGMENT_TAG_LOGIN);
                    if (fragment instanceof LoginFragment)
                        ((LoginFragment) fragment).onAuthenteqLoginResult(result.isSuccessful());
                } else {
                    // process is canceled by user
                }
                break;
            default:
                super.onActivityResult(requestCode, resultCode, data);
        }
    }
}
