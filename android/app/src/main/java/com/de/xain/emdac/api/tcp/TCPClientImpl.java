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

package com.de.xain.emdac.api.tcp;

import android.support.annotation.WorkerThread;
import android.util.Pair;

import com.de.xain.emdac.api.APILibDacAuthNative;
import com.de.xain.emdac.di.AppSharedPreferences;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.util.ArrayDeque;
import java.util.Queue;

import javax.inject.Inject;
import javax.inject.Singleton;

import io.reactivex.Observable;
import io.reactivex.subjects.PublishSubject;
import timber.log.Timber;

import static com.de.xain.emdac.SettingsFragment.Keys.PREF_KEY_IP_ADDRESS;
import static com.de.xain.emdac.SettingsFragment.Keys.PREF_KEY_PORT_NUMBER;
import static com.de.xain.emdac.SettingsFragment.Keys.PREF_KEY_PROTOCOL;

/**
 * Helper class for managing TCP connection
 */
@Singleton
public class TCPClientImpl implements TCPClient {

    private final int PROTOCOL_NONE = 0;
    private final int PROTOCOL_DAC = 1;
    private final int PROTOCOL_TINY = 2;

    private final int BUFFER_SIZE = 4096;

    private PublishSubject<Pair<String, String>> mResponse = PublishSubject.create();
    private PublishSubject<TCPError> mTCPError = PublishSubject.create();

    private Queue<String> mMessageQueue = new ArrayDeque<>();
    private AppSharedPreferences mPreferences;
    // while this is true, the server will continue running
    private boolean mRun = false;
    private TCPSocketObject mTcpSocketObject;
    private APILibDacAuthNative mApiLibDacAuthNative;
    // used to store pointer to native data structure
    private long[] mSession;
    // used to send messages
    private PrintWriter mBufferOut;
    // used to read messages from the server
    private BufferedReader mBufferIn;
    private TCPClientWorker mWorker;
//    private ErrorListener mErrorListener;

    @Inject
    public TCPClientImpl(AppSharedPreferences sharedPreferences, APILibDacAuthNative apiLibDacAuthNative) {
        mPreferences = sharedPreferences;
        mApiLibDacAuthNative = apiLibDacAuthNative;
        mWorker = new TCPClientWorker();
    }

    public void sendMessage(final String message) {
        switch (mPreferences.getInt(PREF_KEY_PROTOCOL)) {
            case PROTOCOL_TINY:
                doTinyConnection(message);
                break;
            case PROTOCOL_DAC:
            case PROTOCOL_NONE:
                doSimpleConnection(message);
                break;
        }
    }

    @Override
    public Observable<Pair<String, String>> getObservableResponse() {
        return mResponse;
    }

    @Override
    public Observable<TCPClient.TCPError> getObservableTCPError() {
        return mTCPError;
    }

    private void sendTcpMessage(final String message) {
        Timber.d("Sending TCP message: %s", message);
        String modifiedMessage = message + "\0";
        Runnable runnable;
        switch (mPreferences.getInt(PREF_KEY_PROTOCOL)) {
            case PROTOCOL_TINY:
                runnable = () -> {
                    mMessageQueue.add(message);
                    byte[] data = modifiedMessage.getBytes();
                    mApiLibDacAuthNative.dacSend(mSession, data, (short) data.length);
                };
                break;
            case PROTOCOL_DAC:
            case PROTOCOL_NONE:
            default:
                runnable = () -> {
                    if (mBufferOut != null) {
                        mMessageQueue.add(message);
                        mBufferOut.println(modifiedMessage);
                        mBufferOut.flush();
                    } else {
                        closeConnection();
                        mTCPError.onNext(TCPError.UNKNOWN);
                    }
                };
                break;
        }
        mWorker.execute(runnable);
    }

    /**
     * Receives message from server.
     */
    private void receiveTcpMessage() {
        Runnable runnable;
        switch (mPreferences.getInt(PREF_KEY_PROTOCOL)) {
            case PROTOCOL_TINY:
                runnable = () -> {
                    if (!mRun) return;
                    byte[] data = new byte[BUFFER_SIZE];
                    mApiLibDacAuthNative.dacReceive(mSession, data, (short) BUFFER_SIZE);
                    String receivedMessage = new String(data);

                    String sentMessage = mMessageQueue.poll();
                    if (sentMessage != null) {
                        mResponse.onNext(new Pair<>(sentMessage, receivedMessage));
                    }

                };
                break;
            case PROTOCOL_DAC:
            case PROTOCOL_NONE:
            default:
                runnable = () -> {
                    StringBuilder message = new StringBuilder();
                    int c;
                    try {
                        while ((mBufferIn != null) && ((c = mBufferIn.read()) != '\0')) {
                            message.append(Character.toString((char) c));
                        }
                    } catch (IOException ex) {
                        ex.printStackTrace();
                        Timber.e("Buffer reading error!");
                    }
                    Timber.d("S: Received Message: '%s'", message.toString());
                    String sentMessage = mMessageQueue.poll();
                    if (sentMessage != null) {
                        mResponse.onNext(new Pair<>(sentMessage, message.toString()));
                    }

                };
                break;
        }
        mWorker.execute(runnable);
    }

    /**
     * Close the connection and release the members
     */
    public synchronized void closeConnection() {

        Timber.d("Stopping client");

        mRun = false;

        if (mApiLibDacAuthNative != null && mPreferences.getInt(PREF_KEY_PROTOCOL) == PROTOCOL_TINY && mSession != null) {
            mApiLibDacAuthNative.dacRelease(mSession);
            mSession = null;
        }

        // close IN and OUT buffers
        if (mBufferOut != null) {
            mBufferOut.flush();
            mBufferOut.close();
            mBufferOut = null;
        }
        mBufferIn = null;

        if (mTcpSocketObject != null) {
            try {
                mTcpSocketObject.close();
            } catch (IOException e) {
                Timber.e("Error %s", e.toString());
            }
            mTcpSocketObject = null;
        }

        Timber.d("TCP connection stopped");
    }

    public synchronized void cancelAllTasks() {
        closeConnection();
        mWorker.interrupt();
        mWorker = new TCPClientWorker();
    }

    /**
     * Sends message through Tiny Embedded
     *
     * @param message message to be sent
     */
    private void doTinyConnection(String message) {
        try {
            initConnection();
        } catch (TCPClientException e) {
            Timber.e(e);
            mTCPError.onNext(TCPError.UNABLE_TO_CONNECT);
            return;
        }
        mSession = new long[1];
        //noinspection ConstantConditions
        mSession[0] = 0;
        if (mApiLibDacAuthNative.dacInitClient(mSession, mTcpSocketObject) == 1) {
            mApiLibDacAuthNative.dacRelease(mSession);
            mTCPError.onNext(TCPError.UNABLE_TO_CONNECT);
            return;
        }
        if (mApiLibDacAuthNative.dacAuthenticate(mSession) == 1) {
            mApiLibDacAuthNative.dacRelease(mSession);
            mTCPError.onNext(TCPError.UNABLE_TO_CONNECT);
            return;
        }
        mRun = true;

        sendTcpMessage(message);
        receiveTcpMessage();
    }

    /**
     * Does the connection through plain TCP
     */
    private void doSimpleConnection(String message) {
        try {
            initConnection();
        } catch (TCPClientException e) {
            Timber.e(e);
            mTCPError.onNext(TCPError.UNABLE_TO_CONNECT);
            return;
        }

        if (mTcpSocketObject == null) return;

        DataInputStream dataInputStream = mTcpSocketObject.getDataInputStream();
        DataOutputStream dataOutputStream = mTcpSocketObject.getDataOutputStream();

        //sends the message to the server
        mBufferOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(dataOutputStream)), true);
        //receives the message which the server sends back
        mBufferIn = new BufferedReader(new InputStreamReader(dataInputStream));
        mRun = true;

        sendTcpMessage(message);
        receiveTcpMessage();
    }

    @WorkerThread
    private void initConnection() throws TCPClientException {
        try {
            InetAddress serverAddress = InetAddress.getByName(mPreferences.getString(PREF_KEY_IP_ADDRESS));
            mTcpSocketObject = new TCPSocketObject(serverAddress, mPreferences.getInt(PREF_KEY_PORT_NUMBER));
            mBufferIn = new BufferedReader(new InputStreamReader(mTcpSocketObject.getDataInputStream()));
            mBufferOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(mTcpSocketObject.getDataOutputStream())), true);
        } catch (Exception e) {
            throw new TCPClientException(e);
        }
    }

    public boolean isConnected() {
        return mRun;
    }

}
