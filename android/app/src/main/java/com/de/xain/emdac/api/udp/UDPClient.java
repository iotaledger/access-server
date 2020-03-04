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

package com.de.xain.emdac.api.udp;

import android.util.Log;

import com.de.xain.emdac.api.AbstractClient;
import com.de.xain.emdac.api.OnMessageReceived;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.nio.charset.Charset;

import javax.inject.Inject;
import javax.inject.Singleton;

import timber.log.Timber;

/**
 * Class representing the UDP client
 */
@Singleton
public class UDPClient extends AbstractClient implements Runnable {

    private final int UDP_SERVER_PORT = 9997;
    private boolean mKeepRunning = true;
//    private DatagramChannel mChannel;
    private DatagramSocket mSocket;
//    private final String TAG = UDPClient.class.getSimpleName();

    /**
     * Constructor of the class. OnMessagedReceived listens for the messages received from server
     */
    @Inject
    public UDPClient(OnMessageReceived listener) {
        mMessageListener = listener;
    }

    /**
     * Listens for UDP broadcasts
     */
    public void run() {
        mKeepRunning = true;
        Timber.d("Connecting UDP listener");
        String message;

        try {
            // try connection through UDP
            mSocket = new DatagramSocket(UDP_SERVER_PORT);
            Timber.d("UDP connection successful, listening for UDP broadcasts");
            // listen for UDP broadcasts
            while (mKeepRunning) {
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                mSocket.receive(packet);
                message = new String(packet.getData(), 0, packet.getLength());
                mMessageListener.messageReceived(message);
                Timber.d("UDP message received: %s", message);
            }
        } catch (SocketException socketException) {
            Timber.e("UDP connection closed");
        } catch (Throwable e) {
            e.printStackTrace();
            Timber.e("UDP connection failed");
        }
    }

    public void stopClient() {
        Timber.d("UDP connection stopping");
        mKeepRunning = false;
        if (mSocket != null) {
            mSocket.close();
        }
//        if (mChannel != null) {
//            try {
//                mChannel.disconnect();
//            } catch (IOException e) {
//                e.printStackTrace();
//                Timber.e("Unable to disconnect UDP channel!");
//            }
//        }
    }
}
