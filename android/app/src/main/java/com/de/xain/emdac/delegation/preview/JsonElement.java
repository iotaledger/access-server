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

package com.de.xain.emdac.delegation.preview;

import android.support.annotation.Nullable;
import android.widget.ArrayAdapter;

import com.de.xain.emdac.utils.ui.recursiverecyclerview.RecursiveItem;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

public class JsonElement implements RecursiveItem {

    private String key;
    @Nullable
    private String value;
    @Nullable
    private JSONObject json;
    @Nullable
    private JSONArray jsonArray;

    public JsonElement(String key, String value, JSONObject json, JSONArray jsonArray) {
        this.key = key;
        this.value = value;
        this.json = json;
        this.jsonArray = jsonArray;
    }

    public String getKey() {
        return key;
    }

    @Nullable
    public String getValue() {
        return value;
    }

    @Nullable
    public JSONObject getJson() {
        return json;
    }

    //    public

    @Override
    public List<RecursiveItem> getChildren() {
        List<RecursiveItem> list = new ArrayList<>();
        if (jsonArray != null) {
            list = Arrays.asList(makeList(jsonArray).toArray(new JsonElement[0]));
        }
        if (value != null) return list;
        if (json == null) return list;
        list = Arrays.asList(makeList(json).toArray(new JsonElement[0]));
        return list;
    }

    public static List<JsonElement> makeList(JSONObject json) {
        Iterator<String> keys = json.keys();
        List<JsonElement> items = new ArrayList<>();
        while (keys.hasNext()) {
            String key = keys.next();
            try {
                Object object = json.get(key);
                if (object instanceof String) {
                    items.add(new JsonElement(key, (String) object, null, null));
                } else if (object instanceof Integer) {
                    items.add(new JsonElement(key, String.valueOf(object), null, null));
                } else if (object instanceof JSONObject) {
                    items.add(new JsonElement(key, null, (JSONObject) object, null));
                } else if (object instanceof JSONArray) {
                    items.add(new JsonElement(key, null, null, (JSONArray) object));
                }
            } catch (JSONException ignored) {
            }
        }

        return items;
    }

    public static List<JsonElement> makeList(JSONArray json) {
        List<JsonElement> items = new ArrayList<>();
        for (int i = 0, n = json.length(); i < n; ++i) {
            try {
                JSONObject object = json.getJSONObject(i);
                items.addAll(makeList(object));
            } catch (JSONException ignored) {
                try {
                    JSONArray object = json.getJSONArray(i);
                    items.addAll(makeList(object));
                } catch (JSONException e1) {
                    e1.printStackTrace();
                }
            }
        }
        return items;
    }
}
