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

import android.content.res.Resources;
import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.content.ContextCompat;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.de.xain.emdac.databinding.ActivityBaseBinding;
import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.utils.ui.BackPressHandler;
import com.de.xain.emdac.utils.ui.Theme;
import com.de.xain.emdac.utils.ui.ThemeLab;
import com.de.xain.emdac.utils.ui.UiUtils;

import javax.inject.Inject;

import dagger.android.AndroidInjection;
import dagger.android.AndroidInjector;
import dagger.android.DispatchingAndroidInjector;
import dagger.android.support.HasSupportFragmentInjector;

import static com.de.xain.emdac.SettingsFragment.Keys.PREF_KEY_THEME;

/**
 * Base activity for providing the toolbar on every activity that extends it
 */
public abstract class BaseActivity extends AppCompatActivity implements HasSupportFragmentInjector {

    protected Toolbar mToolbar;

    protected TextView mToolbarTitle;

    @Inject
    DispatchingAndroidInjector<Fragment> mDispatchingAndroidInjector;

    @Inject
    AppSharedPreferences mPreferences;

    @Inject
    ThemeLab mThemeLab;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        AndroidInjection.inject(this);

        ActivityBaseBinding binding = DataBindingUtil.setContentView(this, R.layout.activity_base);
        mToolbar = binding.toolbar;
        mToolbarTitle = binding.toolbarTitle;

        // set theme from shared preferences
        setTheme(mThemeLab.getTheme(mPreferences.getInt(PREF_KEY_THEME)));

        setToolbar();

        super.onCreate(savedInstanceState);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        for (int i = 0, n = menu.size(); i < n; i++) {
            MenuItem item = menu.getItem(i);
            Drawable icon = item.getIcon();
            if (icon == null) continue;

            icon = DrawableCompat.wrap(icon);
            DrawableCompat.setTint(icon, UiUtils.getColorFromAttr(BaseActivity.this, R.attr.action_menu_icon_color, Color.GRAY));
        }
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public Resources.Theme getTheme() {
        Resources.Theme theme = super.getTheme();
        theme.applyStyle(mThemeLab.getTheme(mPreferences.getInt(PREF_KEY_THEME)).getThemeId(), true);
        return theme;
    }

    protected void setTheme(Theme theme) {
        setTheme(theme.getThemeId());

        if (mToolbarTitle != null) {

            // set title logo
            if (theme.getLogoId() != null) {
                mToolbarTitle.setCompoundDrawablesWithIntrinsicBounds(theme.getLogoId(), 0, 0, 0);
            } else {
                mToolbarTitle.setCompoundDrawablesWithIntrinsicBounds(null, null, null, null);
            }

            // set title
            if (theme.getTitleId() != null) {
                mToolbarTitle.setText(theme.getTitleId());
            } else {
                mToolbarTitle.setText(null);
            }
            mToolbarTitle.setPadding(0, 0, 0, 0);
        }
    }

    @Override
    public void onBackPressed() {
        Fragment currentFragment = getCurrentFragment();

        // check if fragment handles on back press itself
        if (currentFragment instanceof BackPressHandler) {
            ((BackPressHandler) currentFragment).handleOnBackPress();
        } else {
            FragmentManager fm = getSupportFragmentManager();
            if (fm.getBackStackEntryCount() > 1) {
                fm.popBackStack();
            } else {
                finish();
            }
        }

    }

    @Nullable
    protected Fragment getCurrentFragment() {
        FragmentManager fragmentManager = getSupportFragmentManager();
        if (fragmentManager != null && fragmentManager.getBackStackEntryCount() > 0) {
            String fragmentTag = fragmentManager.getBackStackEntryAt(fragmentManager.getBackStackEntryCount() - 1).getName();
            return fragmentManager.findFragmentByTag(fragmentTag);
        } else {
            return null;
        }
    }

    protected void setToolbar() {
        setSupportActionBar(mToolbar);
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowTitleEnabled(false);
        }
    }

    public void addFragmentToBackStack(@NonNull Fragment fragment, @NonNull String tag) {
        getSupportFragmentManager()
                .beginTransaction()
                .addToBackStack(tag)
                .setCustomAnimations(android.R.animator.fade_in, 0)
                .replace(R.id.activity_container, fragment, tag)
                .commitAllowingStateLoss();
    }

    @Override
    public AndroidInjector<Fragment> supportFragmentInjector() {
        return mDispatchingAndroidInjector;
    }
}
