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

package com.de.xain.emdac.user;

import android.support.annotation.Nullable;

import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.models.User;

import javax.inject.Inject;

public class UserManager {

    AppSharedPreferences mPreferences;

    private User mUser;

    @Inject
    public UserManager(AppSharedPreferences preferences) {
        mPreferences = preferences;
        mUser = mPreferences.getUser();
    }

    @Nullable
    public User getUser() {
        return mUser;
    }

    public void startSession(User user) {
        mUser = user;
        mPreferences.putUser(user);
    }

    public void endSession() {
        mUser = null;
        mPreferences.putUser(null);
    }

    public boolean isUserLoggedIn() {
        return mUser != null;
    }
}
