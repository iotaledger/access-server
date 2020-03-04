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

package com.de.xain.emdac.splash;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.api.model.policy.PolicyAttribute;
import com.de.xain.emdac.api.model.policy.PolicyAttributeList;
import com.de.xain.emdac.api.model.policy.PolicyAttributeList.Operation;
import com.de.xain.emdac.api.model.policy.PolicyAttributeSingle;
import com.de.xain.emdac.login.LoginActivity;
import com.de.xain.emdac.navigation.NavigationDrawerActivity;
import com.de.xain.emdac.user.UserManager;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.inject.Inject;

import dagger.android.AndroidInjection;
import timber.log.Timber;

public class SplashActivity extends AppCompatActivity {

    static {
        System.loadLibrary("native-lib");
    }

    @Inject
    UserManager mUserManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AndroidInjection.inject(this);
        setContentView(R.layout.activity_splash);

        // check if user has already logged in
        if (mUserManager.isUserLoggedIn()) {
            // user has logged in
            startActivity(NavigationDrawerActivity.newIntent(this));
        } else {
            // user has NOT logged in
            startActivity(LoginActivity.newIntent(this));
        }
        finish();
    }
}
