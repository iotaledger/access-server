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

package com.de.xain.emdac.navigation;

import android.content.Context;
import android.content.Intent;
import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.support.design.widget.NavigationView;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceFragmentCompat;
import android.view.Menu;
import android.view.MenuItem;

import com.de.xain.emdac.BaseActivity;
import com.de.xain.emdac.R;
import com.de.xain.emdac.SettingsFragment;
import com.de.xain.emdac.api.TSService;
import com.de.xain.emdac.api.model.token_server.TSBalanceRequest;
import com.de.xain.emdac.api.model.token_server.TSBalanceResponse;
import com.de.xain.emdac.api.model.token_server.TSDataResponse;
import com.de.xain.emdac.databinding.ActivityNavigationDrawerBinding;
import com.de.xain.emdac.databinding.NavigationHeaderBinding;
import com.de.xain.emdac.delegation.DelegationFragment;
import com.de.xain.emdac.delegation.Rule;
import com.de.xain.emdac.delegation.RuleInformable;
import com.de.xain.emdac.delegation.rule.DelegationRuleFragment;
import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.log.LogFragment;
import com.de.xain.emdac.login.LoginActivity;
import com.de.xain.emdac.main.ui.CommandListFragment;
import com.de.xain.emdac.main.ui.VehicleInfoListFragment;
import com.de.xain.emdac.models.User;
import com.de.xain.emdac.user.UserManager;
import com.de.xain.emdac.utils.ui.DialogFragmentUtil;
import com.de.xain.emdac.utils.ui.Theme;
import com.de.xain.emdac.utils.ui.ThemeLab;
import com.de.xain.emdac.utils.ui.UiUtils;

import javax.inject.Inject;

import dagger.android.AndroidInjection;
import dagger.android.AndroidInjector;
import dagger.android.DispatchingAndroidInjector;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import timber.log.Timber;

import static com.de.xain.emdac.utils.Constants.TOKEN_SCALE_FACTOR;

public class NavigationDrawerActivity extends BaseActivity implements
        PreferenceFragmentCompat.OnPreferenceStartFragmentCallback,
        DelegationRuleFragment.InternalRuleDelegator {

    private static final String FRAGMENT_TAG_COMMANDS_LIST = "com.de.xain.emdac.fragment_commands_list";
    private static final String FRAGMENT_TAG_SETTINGS = "com.de.xain.fragment_settings";
    private static final String FRAGMENT_TAG_DELEGATION_EDITOR = "com.de.xain.fragment_delegation_editor";
    private static final String FRAGMENT_TAG_VEHICLE_INFORMATION = "com.de.xain.fragment_vehicle_information";
    private static final String FRAGMENT_TAG_LOG = "com.de.xain.fragment_log";
    private static final String FRAGMENT_LOG_OUT = "com.de.xain.fragment_log_out";
    @Inject
    UserManager mUserManager;
    @Inject
    AppSharedPreferences mPreferences;
    @Inject
    DispatchingAndroidInjector<Fragment> mDispatchingAndroidInjector;
    @Inject
    ThemeLab mThemeLab;
    @Inject
    TSService mTSService;

    private Call<TSDataResponse<TSBalanceResponse>> mBalanceResponseCall;
    private DrawerLayout mDrawerLayout;

    private NavigationView mNavigationView;

    private NavigationHeaderBinding mHeaderBinding;

    public static Intent newIntent(Context context) {
        return new Intent(context, NavigationDrawerActivity.class);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        AndroidInjection.inject(this);
        ActivityNavigationDrawerBinding binding = DataBindingUtil
                .setContentView(this, R.layout.activity_navigation_drawer);

        mDrawerLayout = binding.drawerLayout;
        mNavigationView = binding.navigationDrawerView;
        mToolbarTitle = binding.activityBaseLayout.toolbarTitle;
        mToolbar = binding.activityBaseLayout.toolbar;
        mHeaderBinding = DataBindingUtil.bind(mNavigationView.getHeaderView(0));
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            mHeaderBinding.getRoot().setElevation(10f);
        }

        setTheme(mThemeLab.getTheme(mPreferences));

        setDrawer();
        setToolbar();

        if (savedInstanceState == null) {
            mNavigationView.setCheckedItem(R.id.home_menu_item);
        }

        Fragment fragment = getCurrentFragment();
        if (fragment == null) {
            fragment = CommandListFragment.newInstance();
            addFragmentToBackStack(fragment, FRAGMENT_TAG_COMMANDS_LIST);
        }

        mHeaderBinding.refreshButton.setOnClickListener(__ -> getBalance());
        getBalance();
    }

    public void getBalance() {
        if (mBalanceResponseCall != null && !mBalanceResponseCall.isExecuted()) {
            return;
        }
        TSBalanceRequest balanceRequest = new TSBalanceRequest(mUserManager.getUser().getWalletId());
        mBalanceResponseCall = mTSService.getBalance(balanceRequest);

        mBalanceResponseCall.enqueue(new Callback<TSDataResponse<TSBalanceResponse>>() {
            @Override
            public void onResponse(Call<TSDataResponse<TSBalanceResponse>> call, Response<TSDataResponse<TSBalanceResponse>> response) {
                Float numOfTokens = null;

                // extract body
                if (response.body() != null && response.body().getData() != null) {
                    TSBalanceResponse balanceResponse = response.body().getData();
                    numOfTokens = balanceResponse.getBalance();
                }

                String numOfTokensString;
                if (numOfTokens == null) {
                    numOfTokensString = getString(R.string.label_unknown_number_of_tokens);
                } else {
                    numOfTokensString = getResources().getQuantityString(R.plurals.token_plural, 2, String.valueOf((int) (numOfTokens * TOKEN_SCALE_FACTOR)));
                }

                mHeaderBinding.textNumOfTokens.setText(numOfTokensString);
            }

            @Override
            public void onFailure(Call<TSDataResponse<TSBalanceResponse>> call, Throwable t) {

            }
        });

//        mBalanceResponseCall.enqueue(new Callback<BalanceResponse>() {
//            @Override
//            public void onResponse(@NonNull Call<BalanceResponse> call, @NonNull Response<BalanceResponse> response) {
//
//                Float numOfTokens = null;
//
//                // extract body
//                if (response.body() != null && response.body().getBalance() != null) {
//                    numOfTokens = response.body().getBalance();
//                }
//
//                String numOfTokensString;
//                if (numOfTokens == null) {
//                    numOfTokensString = getString(R.string.label_unknown_number_of_tokens);
//                } else {
//                    numOfTokensString = getResources().getQuantityString(R.plurals.token_plural, 2, String.valueOf((int) (numOfTokens * TOKEN_SCALE_FACTOR)));
//                }
//
//                mHeaderBinding.textNumOfTokens.setText(numOfTokensString);
//            }
//
//            @Override
//            public void onFailure(@NonNull Call<BalanceResponse> call, @NonNull Throwable t) {
//            }
//        });
    }

    @Override
    protected void setTheme(Theme theme) {
        super.setTheme(theme);
        if (mHeaderBinding == null) return;

        if (theme.getLargeLogoId() != null) {
            mHeaderBinding.imageLogo.setImageResource(theme.getLargeLogoId());
        }

        Drawable refreshIcon = UiUtils.getCompatDrawable(this, R.drawable.ic_refresh);
        if (refreshIcon != null) {
            refreshIcon = refreshIcon.mutate();
            refreshIcon.setTint(UiUtils.getColorFromAttr(this, R.attr.action_menu_icon_color, Color.GRAY));
            mHeaderBinding.refreshButton.setBackground(refreshIcon);
        }
    }

    @Override
    protected void setToolbar() {
        super.setToolbar();
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {

            actionBar.setDisplayHomeAsUpEnabled(true);
            Drawable homeIcon = UiUtils.getCompatDrawable(this, R.drawable.ic_menu);
            if (homeIcon != null) {
                homeIcon.setTint(UiUtils.getColorFromAttr(this, R.attr.action_menu_icon_color, Color.GRAY));
                actionBar.setHomeAsUpIndicator(homeIcon);
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                mDrawerLayout.openDrawer(GravityCompat.START);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void setDrawer() {
        mNavigationView.setNavigationItemSelectedListener(menuItem -> {
            if (menuItem.equals(mNavigationView.getCheckedItem())) {
                mDrawerLayout.closeDrawers();
                return false;
            }
            switch (menuItem.getItemId()) {
                case R.id.home_menu_item:
                    popBackStackToHome();
                    break;
                case R.id.settings_manu_item:
                    popBackStackToHome();
                    addFragmentToBackStack(SettingsFragment.newInstance(true), FRAGMENT_TAG_SETTINGS);
                    break;
                case R.id.delegation_editor_menu_item:
                    popBackStackToHome();
                    addFragmentToBackStack(DelegationFragment.newInstance(), FRAGMENT_TAG_DELEGATION_EDITOR);
                    break;
                case R.id.log_menu_item:
                    popBackStackToHome();
                    addFragmentToBackStack(LogFragment.newInstance(), FRAGMENT_TAG_LOG);
                    break;
                case R.id.log_out_menu_item:
                    logOut();
                    menuItem.setChecked(false);
                    mDrawerLayout.closeDrawers();
                    return false;
            }
            menuItem.setChecked(true);
            mDrawerLayout.closeDrawers();
            return false;
        });

        User user = mUserManager.getUser();
        if (user != null) {
            mHeaderBinding.textUsername.setText(user.getFirstName());
//            String numOfTokens = getResources().getQuantityString(
//                    R.plurals.token_plural,
//                    user.getNumOfTokens(),
//                    user.getNumOfTokens());
//            mHeaderBinding.textNumOfTokens.setText(numOfTokens);
            mHeaderBinding.refreshButton.setOnClickListener(__ -> getBalance());
        }
    }

    /**
     * Call this when changing navigation drawer items to pop all items
     * on fragment's manager back stack.
     */
    private void popBackStackToHome() {
        FragmentManager fm = getSupportFragmentManager();
        // decrease entry count to always leave "home fragment" on back stack
        for (int i = 0; i < fm.getBackStackEntryCount() - 1; ++i) {
            fm.popBackStack();
        }
    }

    @Override
    public AndroidInjector<Fragment> supportFragmentInjector() {
        return mDispatchingAndroidInjector;
    }

    @Override
    public boolean onPreferenceStartFragment(PreferenceFragmentCompat caller, Preference preference) {
        if (preference.getFragment().equals(VehicleInfoListFragment.class.getName())) {
            VehicleInfoListFragment fragment = VehicleInfoListFragment.newInstance();
            addFragmentToBackStack(fragment, FRAGMENT_TAG_VEHICLE_INFORMATION);
            return true;
        }
        return false;
    }

    @Override
    public void onBackPressed() {
        // check if drawer is opened
        if (mDrawerLayout.isDrawerOpen(mNavigationView)) {
            mDrawerLayout.closeDrawers();
        } else {
            super.onBackPressed();
        }
        FragmentManager fm = getSupportFragmentManager();
        fm.executePendingTransactions();
        int count = fm.getBackStackEntryCount();
        if (count == 1) {
            mNavigationView.setCheckedItem(R.id.home_menu_item);
        }
    }

    /**
     * Logs out user and redirects him to Login screen.
     */
    private void logOut() {
        DialogFragment logoutDialog = DialogFragmentUtil.createAlertDialog(
                getString(R.string.msg_logout),
                android.R.string.yes,
                android.R.string.no,
                __ -> {
                    mUserManager.endSession();
                    startActivity(new Intent(NavigationDrawerActivity.this, LoginActivity.class));
                    Timber.d("Successfully logged out");
                    finish();
                });
        DialogFragmentUtil.showDialog(logoutDialog, getSupportFragmentManager(), FRAGMENT_LOG_OUT);
    }

    public void reloadCommandsFromPreferences() {
        FragmentManager fm = getSupportFragmentManager();
        Fragment fragment = fm.findFragmentByTag(FRAGMENT_TAG_COMMANDS_LIST);
        if (fragment instanceof CommandListFragment)
            ((CommandListFragment) fragment).combineCommandsFromServerAndFromPreferences();
    }

    @Override
    public void delegateNewRule(Rule rule, String tag) {
        FragmentManager fm = getSupportFragmentManager();
        if (fm == null)
            return;
        Fragment fragment = fm.findFragmentByTag(tag);
        if (fragment == null)
            return;
        if (fragment instanceof RuleInformable)
            ((RuleInformable) fragment).onNewRuleCreated(rule);
    }

    @Override
    public void delegateEditedRule(Rule rule, String tag) {
        FragmentManager fm = getSupportFragmentManager();
        if (fm == null)
            return;
        Fragment fragment = fm.findFragmentByTag(tag);
        if (fragment == null)
            return;
        if (fragment instanceof RuleInformable)
            ((RuleInformable) fragment).onRuleEdited(rule);
    }

    @Override
    public void delegateRuleDeleted(Rule rule, String tag) {
        FragmentManager fm = getSupportFragmentManager();
        if (fm == null)
            return;
        Fragment fragment = fm.findFragmentByTag(tag);
        if (fragment == null)
            return;
        if (fragment instanceof RuleInformable)
            ((RuleInformable) fragment).onRuleDelete(rule);
    }
}
