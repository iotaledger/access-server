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

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

public class RegisterUserModel implements Serializable {

    @SerializedName("firstName")
    @Expose
    private String mFirstName;

    @SerializedName("lastName")
    @Expose
    private String mLastName;

    @SerializedName("username")
    @Expose
    private String mUsername;

    @SerializedName("userId")
    @Expose
    private String mUserId;

    public RegisterUserModel(String firstName, String lastName, String username, String userId) {
        mFirstName = firstName;
        mLastName = lastName;
        mUsername = username;
        mUserId = userId;
    }


}
