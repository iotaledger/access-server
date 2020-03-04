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

package com.de.xain.emdac.utils;

import android.support.annotation.Nullable;

import com.google.gson.JsonElement;
import com.google.gson.JsonParser;
import com.google.gson.JsonSyntaxException;

public class JsonUtils {

    @Nullable
    public static JsonElement extractJsonElement(String json) {
        char[] chars = json.toCharArray();
        String balancedString = null;

        if (chars.length == 0) return null;
        if (chars[0] == '{') {
            balancedString = balanceOpeningsAndClosings(json.toCharArray(), '{', '}');
        } else if (chars[0] == '[') {
            balancedString = balanceOpeningsAndClosings(json.toCharArray(), '[', ']');
        }

        if (balancedString == null)
            return null;

        try {
            return new JsonParser().parse(balancedString);
        } catch (JsonSyntaxException ignored) {
            return null;
        }
    }

    @Nullable
    public static String balanceOpeningsAndClosings(char[] input, char openChar, char closeChar) {
        int balance = 0;
        int start = -1;
        int end = -1;
        boolean isBalanced = false;

        for (int i = 0; i < input.length; i++) {
            if (input[i] == openChar) {
                if (start == -1)
                    start = i;
                balance++;
            }
            else if (input[i] == closeChar) {
                end = i;
                balance--;
            }

            if ((balance == 0) && (start != -1) && (end != -1) && (start < end)) {
                isBalanced = true;
                break;
            }
        }

        if (isBalanced) {
            return new String(input, start, end - start + 1);
        } else {
            return null;
        }
    }
}
