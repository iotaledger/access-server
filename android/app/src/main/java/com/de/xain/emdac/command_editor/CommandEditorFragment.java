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

package com.de.xain.emdac.command_editor;

import android.app.Activity;
import android.arch.lifecycle.ViewModelProvider;
import android.arch.lifecycle.ViewModelProviders;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.BaseFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.Command;
import com.de.xain.emdac.databinding.FragmentCommandEditorBinding;
import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.navigation.NavigationDrawerActivity;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import timber.log.Timber;

/**
 * Fragment representing the Command editor screen
 */
public class CommandEditorFragment extends BaseFragment implements Injectable {

    private final String TAG_JSON_ERROR = "json_error_dialog";

    @Inject
    public ViewModelProvider.Factory mViewModelFactory;

    private CommandEditorViewModel mCommandEditorViewModel;
    private FragmentCommandEditorBinding mBinding;
    private CompositeDisposable mDisposable;


    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        mCommandEditorViewModel = ViewModelProviders.of(this, mViewModelFactory).get(CommandEditorViewModel.class);
        mCommandEditorViewModel.restoreState(savedInstanceState);
        mBinding.setViewModel(mCommandEditorViewModel);
    }

    @Override
    public void onResume() {
        super.onResume();
        bindViewModel();
    }

    @Override
    public void onStop() {
        unbindViewModel();
        super.onStop();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putAll(mCommandEditorViewModel.getStateToSave());
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_command_editor, container, false);
        mBinding.buttonCreate.setOnClickListener(__ -> mCommandEditorViewModel.createNewCommand());
        return mBinding.getRoot();
    }

    private void bindViewModel() {
        mDisposable = new CompositeDisposable();
        mDisposable.add(mCommandEditorViewModel
                .getShowDialogMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::showDialogMessage, Timber::e));
        mDisposable.add(mCommandEditorViewModel
                .getShowLoading()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::showLoading, Timber::e));
        mDisposable.add(mCommandEditorViewModel
                .getNewCommand()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::onNewCommandCreate, Timber::e));
        mDisposable.add(mCommandEditorViewModel
                .getSnackbarTextMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(this::showSnackbar, Timber::e));
    }

    private void unbindViewModel() {
        mDisposable.dispose();
    }

    private void showLoading(boolean flag) {
        if (flag) showProgress(R.string.msg_creating_new_command);
        else hideProgress();
    }

    private void onNewCommandCreate(Command newCommand) {
        Activity activity = getActivity();
        if (activity instanceof NavigationDrawerActivity) {
            ((NavigationDrawerActivity) activity).reloadCommandsFromPreferences();
        }
    }
}
