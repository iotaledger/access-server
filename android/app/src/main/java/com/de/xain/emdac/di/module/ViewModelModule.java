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
