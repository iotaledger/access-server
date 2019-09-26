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
