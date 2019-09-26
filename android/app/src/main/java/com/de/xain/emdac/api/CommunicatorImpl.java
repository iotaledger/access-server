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
