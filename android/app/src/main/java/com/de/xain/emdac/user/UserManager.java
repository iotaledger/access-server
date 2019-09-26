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
