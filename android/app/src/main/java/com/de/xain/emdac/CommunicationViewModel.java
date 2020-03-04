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

import android.arch.lifecycle.Lifecycle;
import android.arch.lifecycle.LifecycleObserver;
import android.arch.lifecycle.OnLifecycleEvent;
import android.util.Pair;

import com.de.xain.emdac.api.Communicator;
import com.de.xain.emdac.api.tcp.TCPClient;
import com.de.xain.emdac.utils.ResourceProvider;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.subjects.PublishSubject;
import timber.log.Timber;

public class CommunicationViewModel extends BaseObservableViewModel implements LifecycleObserver {

    protected final Communicator mCommunicator;
    protected final ResourceProvider mResourceProvider;

    protected final PublishSubject<String> mSnackbarMessage = PublishSubject.create();
    protected final PublishSubject<Pair<Boolean, String>> mShowLoading = PublishSubject.create();
    protected final PublishSubject<String> mShowDialogMessage = PublishSubject.create();

    protected CompositeDisposable mCompositeDisposable;

    @Inject
    public CommunicationViewModel(Communicator communicator, ResourceProvider resourceProvider) {
        mCommunicator = communicator;
        mResourceProvider = resourceProvider;
    }

    public Observable<Pair<Boolean, String>> getObservableShowLoadingMessage() {
        return mShowLoading;
    }

    public Observable<String> getObservableShowDialogMessage() {
        return mShowDialogMessage;
    }

    public Observable<String> getObservableSnackbarMessage() {
        return mSnackbarMessage;
    }

    protected void sendTCPMessage(String message) {
        mCommunicator.sendTCPMessage(message);
    }

    protected void sendTCPMessage(String message, String uiMessage) {
        mShowLoading.onNext(new Pair<>(true, uiMessage));
        mCommunicator.sendTCPMessage(message);
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_START)
    protected void onLifecycleEventStart() {
        subscribeForEvents();
    }

    @OnLifecycleEvent(Lifecycle.Event.ON_STOP)
    protected void onLifecycleEventStop() {
        unsubscribeFromEvents();
    }

    protected void subscribeForEvents() {
        mCompositeDisposable = new CompositeDisposable();

        mCompositeDisposable.add(mCommunicator
                .getObservableTCPResponse()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        pair -> handleTCPResponse(pair.first, pair.second),
                        Timber::e
                ));

        mCompositeDisposable.add(mCommunicator
                .getObservableTCPError()
                .observeOn(AndroidSchedulers.mainThread())
                .subscribe(
                        this::handleTCPError,
                        Timber::e
                ));
    }

    protected void unsubscribeFromEvents() {
        if (mCompositeDisposable != null && !mCompositeDisposable.isDisposed()) {
            mCompositeDisposable.dispose();
        }
    }

    protected void handleTCPResponse(String sentMessage, String response) {
        mShowLoading.onNext(new Pair<>(false, null));
        mCommunicator.disconnectTCP();
    }

    protected void handleTCPError(TCPClient.TCPError error) {
        mShowLoading.onNext(new Pair<>(false, null));

        String message;
        switch (error) {
            case UNABLE_TO_CONNECT:
                message = mResourceProvider.getString(R.string.msg_tcp_client_unable_to_connect);
                break;
            case TIMEOUT:
                message = mResourceProvider.getString(R.string.msg_tcp_timeout);
                break;
            case UNKNOWN:
            default:
                message = mResourceProvider.getString(R.string.something_wrong_happened);
                break;
        }
        mSnackbarMessage.onNext(message);
    }

}
