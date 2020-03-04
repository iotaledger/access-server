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

package com.de.xain.emdac;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.support.v7.preference.EditTextPreference;
import android.support.v7.preference.ListPreference;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceCategory;
import android.support.v7.preference.PreferenceScreen;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.ListView;

import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.utils.ui.DisplayUtil;
import com.de.xain.emdac.utils.ui.ThemeLab;
import com.de.xain.emdac.utils.ui.UiUtils;

import javax.inject.Inject;

public class SettingsFragment extends BasePreferenceFragmentCompat implements
        Injectable,
        Preference.OnPreferenceChangeListener {

    public static final String THEME_HAS_CHANGED = "com.de.xain.emdac.theme_has_changed";

    private static final String INITIAL_THEME = "com.de.xain.emdac.initial_theme";
    private static final String SHOW_ADDITIONAL_SETTINGS = "com.de.xain.emdac.show_additional_settings";
    private static final String PREF_CATEGORY_UNITS = "pref_category_units";
    private static final String PREF_CATEGORY_VEHICLE_INFORMATION = "pref_category_vehicle_information";

    @Inject
    ThemeLab mThemeLab;
    private boolean mShowAdditionalSettings = false;
    private String initialTheme;

    public static SettingsFragment newInstance(boolean showAdditionalSettings) {
        SettingsFragment fragment = new SettingsFragment();
        Bundle args = new Bundle();
        args.putBoolean(SHOW_ADDITIONAL_SETTINGS, showAdditionalSettings);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreatePreferences(Bundle bundle, String rootKey) {
        setPreferencesFromResource(R.xml.preferences, rootKey);

        extractArguments();
        PreferenceCategory vehicleInfoCategory = (PreferenceCategory) getPreferenceScreen().findPreference(PREF_CATEGORY_VEHICLE_INFORMATION);
        vehicleInfoCategory.setVisible(mShowAdditionalSettings);

        PreferenceCategory unitsCategory = (PreferenceCategory) getPreferenceScreen().findPreference(PREF_CATEGORY_UNITS);
        unitsCategory.setVisible(mShowAdditionalSettings);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (savedInstanceState != null) {
            initialTheme = savedInstanceState.getString(INITIAL_THEME);
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putString(INITIAL_THEME, initialTheme);
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        Activity activity = getActivity();
        if (activity == null) return;

//        // remove dividers
//        View rootView = getView();
//        if (rootView != null) {
//            ListView list = rootView.findViewById(android.R.id.list);
//            if (list != null) {
//                list.setDivider(ContextCompat.getDrawable(getActivity(), R.drawable.separation_line));
//            }
//        }
//        set
        Drawable divider = UiUtils.getCompatDrawable(getActivity(), R.drawable.separation_line);
        if (divider != null) {
            divider.setTint(UiUtils.getColorFromAttr(activity, R.attr.separator_line_color, Color.GRAY));
            setDivider(divider);
            setDividerHeight(DisplayUtil.convertDensityPixelToPixel(activity, 2));
        }

        extractArguments();

        PreferenceScreen preferenceScreen = getPreferenceScreen();

        // IP address
        EditTextPreference ipAddress;
        ipAddress = (EditTextPreference) preferenceScreen.findPreference(Keys.PREF_KEY_IP_ADDRESS);
        ipAddress.setSummary(ipAddress.getText());
        ipAddress.setOnPreferenceChangeListener(this);

        // port number
        EditTextPreference portNumber;
        portNumber = (EditTextPreference) preferenceScreen.findPreference(Keys.PREF_KEY_PORT_NUMBER);
        portNumber.setSummary(portNumber.getText());
        portNumber.setOnPreferenceChangeListener(this);


        ListPreference theme = (ListPreference) preferenceScreen.findPreference(Keys.PREF_KEY_THEME);
        configListPreference(theme, mThemeLab.getThemeNames(getActivity()));
        if (initialTheme == null)
            initialTheme = theme.getValue();

        configListPreference((ListPreference) preferenceScreen.findPreference(Keys.PREF_KEY_PROTOCOL),
                getResources().getStringArray(R.array.protocol_types));
        configListPreference((ListPreference) preferenceScreen.findPreference(Keys.PREF_KEY_TEMPERATURE_UNIT),
                getResources().getStringArray(R.array.temperature_units));
        configListPreference((ListPreference) preferenceScreen.findPreference(Keys.PREF_KEY_DISTANCE_UNIT),
                getResources().getStringArray(R.array.distance_units));


//        for (int i = 0, n = preferenceScreen.getPreferenceCount(); i < n; i++) {
//            Preference preference = preferenceScreen.getPreference(i);
////            preference.getPreferenceManager().
//            if (preference instanceof EditTextPreference) {
//                EditTextPreference editTextPreference = ((EditTextPreference) preference);
//                editTextPreference.setSummary(editTextPreference.getText());
//                editTextPreference.setOnPreferenceChangeListener(this);
//            }
//        }
    }

    private void configListPreference(ListPreference preference, String[] entryList) {
        preference.setEntries(entryList);
        String[] entryValues = new String[entryList.length];
        for (int i = 0; i < entryValues.length; i++)
            entryValues[i] = String.valueOf(i);
        preference.setEntryValues(entryValues);
        if (entryValues.length > 0 && preference.getEntry() == null)
            preference.setValue(entryValues[0]);
        preference.setSummary(preference.getEntry());
        preference.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object o) {
        if (preference instanceof EditTextPreference) {
            preference.setSummary((CharSequence) o);
            return true;
        }
        if (preference instanceof ListPreference) {
            String newValue = (String) o;
            String oldValue = ((ListPreference) preference).getValue();
            // check if preference is theme related
            if (preference.getKey().equals(Keys.PREF_KEY_THEME)) {
                // check if new value is different from old value
                // this means theme has changed so activity has to be recreated
                if (!newValue.equals(oldValue)) {
                    if (getActivity() != null) {
                        getActivity().recreate();
                    }
                }
            }
            ((ListPreference) preference).setValue((String) o);
            preference.setSummary(((ListPreference) preference).getEntry());
            return false;
        }

        return true;
    }

    private void extractArguments() {
        Bundle args = getArguments();
        if (args == null)
            return;

        if (args.containsKey(SHOW_ADDITIONAL_SETTINGS))
            mShowAdditionalSettings = args.getBoolean(SHOW_ADDITIONAL_SETTINGS);
    }

    @SuppressWarnings("WeakerAccess")
    public static class Keys {
        public static final String PREF_KEY_USER = "pref_user";
        public static final String PREF_KEY_DID_LOGIN = "pref_did_login";
        public static final String PREF_KEY_THEME = "pref_theme";
        public static final String PREF_KEY_LAYER = "pref_layer";
        public static final String PREF_KEY_COMMAND_LIST = "pref_command_list";
        public static final String PREF_KEY_IP_ADDRESS = "pref_ip_address";
        public static final String PREF_KEY_PORT_NUMBER = "pref_port_number";
        public static final String PREF_KEY_PROTOCOL = "pref_protocol";
        public static final String PREF_KEY_USERNAME = "pref_username";
        public static final String PREF_KEY_PASSWORD = "pref_password";
        public static final String PREF_KEY_VEHICLE_INFORMATION = "pref_vehicle_information";
        public static final String PREF_KEY_TEMPERATURE_UNIT = "pref_temperature_unit";
        public static final String PREF_KEY_DISTANCE_UNIT = "pref_distance_unit";
        public static final String PREF_KEY_CUSTOM_COMMANDS = "pref_custom_commands";
    }
}