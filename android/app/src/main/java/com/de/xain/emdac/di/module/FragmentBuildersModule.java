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
