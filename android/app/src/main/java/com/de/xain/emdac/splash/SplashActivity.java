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
