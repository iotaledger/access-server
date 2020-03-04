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

import android.arch.lifecycle.ViewModelProvider;
import android.arch.lifecycle.ViewModelProviders;
import android.os.Bundle;
import android.support.annotation.CallSuper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.di.Injectable;

import javax.inject.Inject;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import timber.log.Timber;

public abstract class CommunicationFragment<T extends CommunicationViewModel> extends BaseFragment implements Injectable {

    @Inject
    public ViewModelProvider.Factory mViewModelFactory;

    protected CompositeDisposable mDisposable;
    protected T mViewModel;
    private boolean isBinded = false;
    abstract protected Class<T> getViewModelClass();

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = ViewModelProviders.of(this, mViewModelFactory).get(getViewModelClass());
        getLifecycle().addObserver(mViewModel);
    }

    @Override
    @CallSuper
    public void onStart() {
        super.onStart();
        if (!isBinded) {
            bindViewModel();
            isBinded = true;
        }
    }

    @Override
    @CallSuper
    public void onDestroy() {
        super.onDestroy();
        unbindViewModel();
    }


    @CallSuper
    protected void bindViewModel() {
        mDisposable = new CompositeDisposable();

        mDisposable.add(mViewModel
                .getObservableShowLoadingMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        pair -> showLoading(pair.first, pair.second),
                        Timber::e));

        mDisposable.add(mViewModel
                .getObservableShowDialogMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::showDialogMessage,
                        Timber::e));

        mDisposable.add(mViewModel
                .getObservableSnackbarMessage()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::showSnackbar,
                        Timber::e));
    }

    @CallSuper
    protected void unbindViewModel() {
        if (mDisposable != null && !mDisposable.isDisposed())
            mDisposable.dispose();
    }

    private void showLoading(boolean show, String message) {
        if (show) {
            showProgress(message, null);
        } else {
            hideProgress();
        }
    }

}
