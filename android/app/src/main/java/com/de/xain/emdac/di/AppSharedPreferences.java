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

package com.de.xain.emdac.di;

import android.content.SharedPreferences;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.de.xain.emdac.api.model.Command;
import com.de.xain.emdac.main.model.VehicleInfo;
import com.de.xain.emdac.models.User;
import com.google.gson.Gson;
import com.google.gson.JsonSyntaxException;
import com.google.gson.reflect.TypeToken;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;
import javax.inject.Singleton;

import static com.de.xain.emdac.SettingsFragment.*;

/**
 * Created by flaviu.lupu on 14-Nov-17.
 * Custom {@link SharedPreferences} class providing different methods for saving data
 */
@Singleton
public class  AppSharedPreferences {

    private final SharedPreferences mSharedPreferences;

    private final Gson mGson;

    @Inject
    AppSharedPreferences(SharedPreferences sharedPreferences, Gson gson) {
        this.mSharedPreferences = sharedPreferences;
        mGson = gson;
    }

    public void putBoolean(String key, boolean value) {
        mSharedPreferences.edit().putBoolean(key, value).apply();
    }

    public boolean getBoolean(String key, boolean defaultValue) {
        return mSharedPreferences.getBoolean(key, defaultValue);
    }

    public void putString(String key, String value) {
        mSharedPreferences.edit().putString(key, value).apply();
    }

    public String getString(String key) {
        return mSharedPreferences.getString(key, "");
    }

    public int getInt(String key) {
        try {
            return mSharedPreferences.getInt(key, 0);
        } catch (Exception ignored) {
            try {
                //noinspection ConstantConditions
                return Integer.valueOf(mSharedPreferences.getString(key, "0"));
            } catch (Exception e) {
                return 0;
            }
        }
    }

    public void putInt(String key, int value) {
        mSharedPreferences.edit().putInt(key, value).apply();
    }

    @Nullable
    public List<VehicleInfo> getVehicleIfnormations() {
        ArrayList<VehicleInfo> arrayList = new ArrayList<>();
        String vehicleInfosString = getString(Keys.PREF_KEY_VEHICLE_INFORMATION);
        if (vehicleInfosString.equalsIgnoreCase("")) { return null; }

        Type type = new TypeToken<List<VehicleInfo>>(){}.getType();
        try {
            return mGson.fromJson(vehicleInfosString, type);
        } catch (JsonSyntaxException ignored) {
            return null;
        }
    }

    public void putVehicleInfromations(List<VehicleInfo> vehicleInfos) {
        putString(Keys.PREF_KEY_VEHICLE_INFORMATION, mGson.toJson(vehicleInfos));
    }

    public void putUser(@Nullable User user) {
        putString(Keys.PREF_KEY_USER, mGson.toJson(user));
    }

    @Nullable
    public User getUser() {
        String userJson = mSharedPreferences.getString(Keys.PREF_KEY_USER, null);
        if (TextUtils.isEmpty(userJson))
            return null;
        try {
            return mGson.fromJson(userJson, User.class);
        } catch (JsonSyntaxException ignored) {
            return null;
        }
    }

    public void putCommandList(List<Command> commandList) {
        putString(Keys.PREF_KEY_CUSTOM_COMMANDS, mGson.toJson(commandList));
    }

    @Nullable
    public List<Command> getCommandList() {
        ArrayList<Command> arrayList = new ArrayList<>();
        String commandListString = getString(Keys.PREF_KEY_CUSTOM_COMMANDS);
        if (commandListString.equalsIgnoreCase("")) { return null; }

        Type type = new TypeToken<List<Command>>(){}.getType();
        try {
            return mGson.fromJson(commandListString, type);
        } catch (JsonSyntaxException ignored) {
            return null;
        }
    }

    public void removeCommandFromList(Command command) {
        // TODO: 30.1.2019. Fix removing command
        List<Command> oldList = getCommandList();
        if (oldList == null) return;
        for (int i = 0, n = oldList.size(); i < n; ++i) {
            if (oldList.get(i).equals(command)) {
                oldList.remove(i);
                break;
            }
        }
        putCommandList(oldList);
    }

}
