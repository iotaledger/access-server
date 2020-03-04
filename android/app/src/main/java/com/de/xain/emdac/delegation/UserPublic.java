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

package com.de.xain.emdac.delegation;

import androidx.annotation.Nullable;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

public class UserPublic implements Serializable {

    @SerializedName("username")
    private String username;
    @SerializedName("publicId")
    private String publicId;

    public UserPublic(String username, String publicId) {
        this.username = username;
        this.publicId = publicId;
    }

    public UserPublic() {
    }

    public String getUsername() {
        return username;
    }

    public String getPublicId() {
        return publicId;
    }

    public String getPublicIdHex() {
        if (publicId == null) return null;
        return "0x" + publicId;
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj instanceof UserPublic) {
            String username = ((UserPublic) obj).username;
            if (username == null) return false;
            return username.equals(this.username);
        } else {
            return false;
        }
    }
}
