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
