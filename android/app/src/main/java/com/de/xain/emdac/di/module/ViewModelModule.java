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

import android.arch.lifecycle.ViewModel;
import android.arch.lifecycle.ViewModelProvider;

import com.de.xain.emdac.CommunicationViewModel;
import com.de.xain.emdac.command_editor.CommandEditorViewModel;
import com.de.xain.emdac.delegation.preview.DelegationPreviewViewModel;
import com.de.xain.emdac.delegation.DelegationViewModel;
import com.de.xain.emdac.delegation.rule.DelegationRuleViewModel;
import com.de.xain.emdac.di.ViewModelKey;
import com.de.xain.emdac.login.LoginViewModel;
import com.de.xain.emdac.main.model.VehicleInfoListViewModel;
import com.de.xain.emdac.main.ui.CommandListViewModel;
import com.de.xain.emdac.register.RegisterViewModel;
import com.de.xain.emdac.viewmodel.CustomViewModelFactory;

import dagger.Binds;
import dagger.Module;
import dagger.multibindings.IntoMap;

/**
 * Helper class for binding custom {@link ViewModelProvider.Factory} and ViewModels
 */
@Module
public abstract class ViewModelModule {

    @Binds
    @IntoMap
    @ViewModelKey(CommandListViewModel.class)
    abstract ViewModel bindCommandListViewModel(CommandListViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(LoginViewModel.class)
    abstract ViewModel bindLoginViewModel(LoginViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(DelegationViewModel.class)
    abstract ViewModel bindDelegationViewModel(DelegationViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(DelegationRuleViewModel.class)
    abstract ViewModel bindDelegationRuleViewModel(DelegationRuleViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(VehicleInfoListViewModel.class)
    abstract ViewModel bindVehicleInfoViewModel(VehicleInfoListViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(CommandEditorViewModel.class)
    abstract ViewModel bindCommandEditorViewModell(CommandEditorViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(DelegationPreviewViewModel.class)
    abstract ViewModel bindDelegationPreviewViewModel(DelegationPreviewViewModel viewModel);

    @Binds
    @IntoMap
    @ViewModelKey(RegisterViewModel.class)
    abstract ViewModel bindRegisterViewModel(RegisterViewModel viewModel);

    @Binds
    abstract ViewModelProvider.Factory bindViewModelFactory(CustomViewModelFactory factory);

    @Binds
    @IntoMap
    @ViewModelKey(CommunicationViewModel.class)
    abstract ViewModel bindCommunicationViewModel(CommunicationViewModel viewModel);
}
