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
