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

package com.de.xain.emdac.di.module;

import com.de.xain.emdac.SettingsFragment;
import com.de.xain.emdac.delegation.DelegationFragment;
import com.de.xain.emdac.delegation.DelegationListFragment;
import com.de.xain.emdac.delegation.preview.DelegationPreviewStructuredFragment;
import com.de.xain.emdac.delegation.rule.DelegationRuleFragment;
import com.de.xain.emdac.login.LoginFragment;
import com.de.xain.emdac.command_editor.CommandEditorFragment;
import com.de.xain.emdac.main.ui.CommandListFragment;
import com.de.xain.emdac.main.ui.VehicleInfoListFragment;
import com.de.xain.emdac.register.RegisterFragment;

import dagger.Module;
import dagger.android.ContributesAndroidInjector;

/**
 * Module class for providing Fragments
 */
@SuppressWarnings({"WeakerAccess", "unused"})
@Module
public abstract class FragmentBuildersModule {
    @ContributesAndroidInjector
    abstract CommandListFragment contributeCommandListFragment();

    @ContributesAndroidInjector
    abstract CommandEditorFragment contributeCommandEditorFragment();

    @ContributesAndroidInjector
    abstract LoginFragment contributeLoginFragment();

    @ContributesAndroidInjector
    abstract VehicleInfoListFragment contributeVehicleInfoFragment();

    @ContributesAndroidInjector
    abstract SettingsFragment contributePreferenceFragment();

    @ContributesAndroidInjector
    abstract DelegationFragment contributeDelegationFragment();

    @ContributesAndroidInjector
    abstract DelegationListFragment contributeDelegationListFragment();

    @ContributesAndroidInjector
    abstract DelegationRuleFragment contributeDelegationRuleFragment();

    @ContributesAndroidInjector
    abstract DelegationPreviewStructuredFragment contributeDelegationPreviewFragment();

    @ContributesAndroidInjector
    abstract RegisterFragment contributeRegisterFragment();
}
