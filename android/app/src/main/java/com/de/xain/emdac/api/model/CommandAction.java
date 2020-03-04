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

package com.de.xain.emdac.api.model;

import android.support.annotation.StringRes;

import com.de.xain.emdac.R;
import com.de.xain.emdac.utils.ResourceProvider;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import androidx.annotation.Nullable;
import timber.log.Timber;

public class CommandAction implements Serializable {

    public static class KnownCommandAction {
        public static final String OPEN_TRUNK_ACTION = "open_trunk";
        public static final String OPEN_DOOR_ACTION = "open_door";
        public static final String CLOSE_DOOR_ACTION = "close_door";
        public static final String START_ENGINE_ACTION = "start_engine";
        public static final String HONK_ACTION = "honk";
        public static final String ALARM_ON = "alarm_on";
        public static final String ALARM_OFF = "alarm_off";
    }

    @SerializedName("policy_id")
    @Expose
    private String policyId;

    @SerializedName("action")
    @Expose
    private String action;

    @SerializedName("cost")
    @Expose
    @Nullable
    private String mCost;

    private String mActionName;
    private String mHeaderName;

    @Override
    public boolean equals(@Nullable Object obj) {
        if (!(obj instanceof CommandAction)) return false;
        CommandAction commandAction = ((CommandAction) obj);
        boolean flag = action.equals(commandAction.action);
        return action.equals(commandAction.action);
    }

    /**
     * @return Returns action name as obtained form server. For user friendly action name,
     * use getActionNameResId() instead.
     */
    public String getAction() {
        return action;
    }

    public void setAction(String action) {
        this.action = action;
    }

    /**
     * @return Return resource Id of user friendly name of action.
     */
    @StringRes
    public int getActionNameResId() {
        switch (action.toLowerCase()) {
            case KnownCommandAction.ALARM_ON:
                return R.string.command_alarm_on;
            case KnownCommandAction.ALARM_OFF:
                return R.string.command_alarm_off;
            case KnownCommandAction.HONK_ACTION:
                return R.string.command_honk;
            case KnownCommandAction.OPEN_TRUNK_ACTION:
                return R.string.command_open_trunk;
            case KnownCommandAction.OPEN_DOOR_ACTION:
                return R.string.command_unlock;
            case KnownCommandAction.CLOSE_DOOR_ACTION:
                return R.string.command_lock;
            case KnownCommandAction.START_ENGINE_ACTION:
                return R.string.start_engine;
            default:
                return 0;
        }
    }

    @StringRes
    public int getHeaderNameResId() {
        switch (getAction().toLowerCase()) {
            case KnownCommandAction.ALARM_ON:
            case KnownCommandAction.ALARM_OFF:
                return R.string.header_alarm;
            case KnownCommandAction.HONK_ACTION:
                return R.string.command_honk;
            case KnownCommandAction.OPEN_TRUNK_ACTION:
                return R.string.header_trunk;
            case KnownCommandAction.OPEN_DOOR_ACTION:
            case KnownCommandAction.CLOSE_DOOR_ACTION:
                return R.string.header_lock;
            case KnownCommandAction.START_ENGINE_ACTION:
                return R.string.start_engine;
            default:
                return 0;
        }
    }

    public String getActionName() {
        return mActionName;
    }

    public String getHeaderName() {
        return mHeaderName;
    }

    public void setHeaderName(String headerName) {
        mHeaderName = headerName;
    }

    public void setActionName(String actionName) {
        mActionName = actionName;
    }

    public String getPolicyId() {
        return policyId;
    }

    @Nullable
    public Float getCost() {
        if (mCost == null) return null;
        try {
            return Float.valueOf(mCost);
        } catch (NumberFormatException ignored) {
            return null;
        }
    }

    public boolean isPaid() {
        if (mCost != null) {
            return mCost.equalsIgnoreCase("0");
        } else {
            return true;
        }
    }

    public void setPaid(boolean paid) {
        if (paid) {
            mCost = null;
        }
    }

    /**
     * Extract action list from JSON array object.
     *
     * @param jsonArray JSON array object containing actions.
     * @return List of actions extracted from JSON array.
     * @throws IllegalStateException Throws JSON parsing exception.
     */
    public static List<CommandAction> getListOfActions(JsonArray jsonArray, ResourceProvider resourceProvider) throws IllegalStateException {
        List<CommandAction> actions = new ArrayList<>();
        Gson gson = new Gson();
        for (int i = 0; i < jsonArray.size(); i++) {
            JsonObject jsonObj;
            CommandAction action;
            jsonObj = jsonArray.get(i).getAsJsonObject();
            action = gson.fromJson(jsonObj.toString(), CommandAction.class);
            if (action.getActionNameResId() == 0) {
                action.setActionName(resourceProvider.getString(R.string.unknown_action));
            } else {
                action.setActionName(resourceProvider.getString(action.getActionNameResId()));
            }
            if (action.getHeaderNameResId() == 0) {
                action.setHeaderName(resourceProvider.getString(R.string.unknown_command));
            } else {
                action.setHeaderName(resourceProvider.getString(action.getHeaderNameResId()));
            }
            actions.add(action);
            Timber.d(action.toString());
        }
        return actions;
    }
}
