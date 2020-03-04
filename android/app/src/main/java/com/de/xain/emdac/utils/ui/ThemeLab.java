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

package com.de.xain.emdac.utils.ui;

import android.content.Context;

import com.de.xain.emdac.R;
import com.de.xain.emdac.di.AppSharedPreferences;

import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;
import javax.inject.Singleton;

import static com.de.xain.emdac.SettingsFragment.Keys.PREF_KEY_THEME;

@Singleton
public class ThemeLab {

    private List<Theme> mThemeList;

    @Inject
    public ThemeLab() {
        mThemeList = generateThemes();
    }

    private List<Theme> generateThemes() {
        List<Theme> themes = new ArrayList<>();

        themes.add(new Theme(R.style.XAIN_NoActionBar,
                R.string.theme_name_xain,
                null,
                R.drawable.ic_svg_xain_main,
                R.drawable.ic_svg_xain_main_large));

        return themes;
    }

    public List<Theme> getThemeList() {
        return mThemeList;
    }

    public String[] getThemeNames(Context context) {
        ArrayList<String> names = new ArrayList<>();
        for (Theme theme : mThemeList) {
            if (theme.getNameId() != null)
                names.add(context.getResources().getString(theme.getNameId()));
        }
        return names.toArray(new String[0]);
    }

    public Theme getTheme(int themePosition) {
        if (themePosition < 0 || themePosition >= mThemeList.size()) {
            return mThemeList.get(0);
        }
        return mThemeList.get(themePosition);
    }

    public Theme getTheme(AppSharedPreferences preferences) {
        int position = preferences.getInt(PREF_KEY_THEME);
        return getTheme(position);
    }

}
