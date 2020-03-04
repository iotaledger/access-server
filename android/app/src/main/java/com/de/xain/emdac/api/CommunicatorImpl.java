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

package com.de.xain.emdac.api;

import android.util.Pair;

import com.de.xain.emdac.api.tcp.TCPClient;
import com.de.xain.emdac.api.tcp.TCPClientImpl;
import com.de.xain.emdac.api.tcp.TCPCommunicationTask;

import javax.inject.Inject;

import io.reactivex.Observable;

public class CommunicatorImpl implements Communicator {
    private final TCPClient mTCPClient;

    @Inject
    public CommunicatorImpl(TCPClient TCPClient) {
        mTCPClient = TCPClient;
    }

    @Override
    public void sendTCPMessage(String message) {
        new TCPCommunicationTask(message).execute(mTCPClient);
    }

    @Override
    public Observable<Pair<String, String>> getObservableTCPResponse() {
        return mTCPClient.getObservableResponse();
    }

    @Override
    public Observable<TCPClient.TCPError> getObservableTCPError() {
        return mTCPClient.getObservableTCPError();
    }
    @Override
    public void disconnectTCP() {
        mTCPClient.closeConnection();
    }
//
//
//    public void sendTCPMessage(final String message) {
//        new TCPCommunicationTask(message).execute(mTCPClient);
//    }
//
//    public Observable<Pair<String, String>> getObservableTCPResponse() {
//        return mTCPClient.getObservableResponse();
//    }
//
//    public Observable<TCPClientImpl.TCPError> getObservableTCPError() {
//        return mTCPClient.getObservableTCPError();
//    }
//
//    public void disconnectTCP() {
//        mTCPClient.closeConnection();
//    }
}
