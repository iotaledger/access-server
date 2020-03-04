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

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

public class UserUtils {

    private static List<User> getDefaultUsers() {
        List<User> userList = new ArrayList<>();

        userList.add(new User(
                "jamie",
                "Jamie",
                "Owner",
                null,
                "0xa41ea875a26Eef4c4319E9998BEa9333cd1d94Cd",
                "3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51",
                "123"));

        userList.add(new User(
                "charlie",
                "Charlie",
                "Family",
                null,
                "0x0fDa9577c6300B273999Ab0429d600D64f843F60",
                "d51d6d76a6002d34af849e52c22719bd2becfee5b0685acd67b0d16654284cf6d200189d22cdab0aafcfd46b055a2e4f5bde31cae849207fd820e03c32ac21f8",
                "123"));

        userList.add(new User(
                "alex",
                "Alex",
                "Friend",
                null,
                "0xa18b2a6e2aE6Cf382b7A263286d47F745f0017f9",
                "df2b3fe3be9f6bd3deca19d275eb76b252389c4c4af2ce33745376357c955f1e0c9d1eb776beacd2bd917fc216787e57156edfb8077c816754a10a46b91a81b2",
                "123"));

        userList.add(new User(
                "richard",
                "Richard",
                "Courier",
                null,
                "0x9A86e646654C8944572216dbe11a4e0DAF98d5d0",
                "90fb6a83806bc167d68d7eaf21e33fa1e8bb6a32ee536091728461e2655be3769cdc4062942ae87d7ed131e880c63914bf7774f282e3220796ffb30c7f3a6bd8",
                "123"));

        return userList;
    }

    @Nullable
    public static User getDefaultUser(@NonNull String username) {
        for (User user : getDefaultUsers()) {
            if (username.equals(user.getUsername())) return user;
        }
        return null;
    }

    public static boolean isDefaultUser(User user) {
        return getDefaultUsers().contains(user);
    }
}
