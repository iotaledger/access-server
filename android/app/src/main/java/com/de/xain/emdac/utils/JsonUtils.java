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
