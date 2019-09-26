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

package com.de.xain.emdac.models;

import androidx.annotation.Nullable;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

public class User implements Serializable {

    @SerializedName("deviceId")
    private final String mDeviceId;

    @SerializedName("username")
    private final String mUsername;

    @SerializedName("userId")
    private final String mUserId;

    @SerializedName("firstName")
    private final String mFirstName;

    @SerializedName("lastName")
    private final String mLastName;

    @SerializedName("publicId")
    private final String mPublicId;

    @SerializedName("walletId")
    private final String mWalletId;

    public User(String username, String firstName, String lastName, String userId, String walletId, String publicId, String deviceId) {
        mUsername = username;
        mFirstName = firstName;
        mLastName = lastName;
        mUserId = userId;
        mWalletId = walletId;
        mPublicId = publicId;
        mDeviceId = deviceId;
    }

    public String getUsername() {
        return mUsername;
    }

    public String getFirstName() {
        return mFirstName;
    }

    public String getUserId() {
        return mUserId;
    }

    public String getWalletId() {
        return mWalletId;
    }

    public String getPublicId() {
        return mPublicId;
    }

    public String getDeviceId() {
        return mDeviceId;
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj instanceof User) {
            if (mUsername != null) {
                return mUsername.equals(((User) obj).mUsername);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}
