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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;

import io.reactivex.annotations.NonNull;
import io.reactivex.annotations.Nullable;

/**
 * Socket class implementation
 */
public class TCPSocketObject {

    private Socket mSocket;
    private DataInputStream mDataInputStream;
    private DataOutputStream mDataOutputStream;

    TCPSocketObject(InetAddress address, int port) throws IOException {
        mSocket = new Socket();

        // connect to address and set timeout to be 3 seconds
        mSocket.connect(new InetSocketAddress(address, port), 3000);

        // sends the message to the server
        mDataOutputStream = new DataOutputStream(mSocket.getOutputStream());

        // receives the message which the server sends back
        mDataInputStream = new DataInputStream(mSocket.getInputStream());
    }

    void close() throws IOException {
        if (mDataOutputStream != null) {
            mDataOutputStream.flush();
            mDataOutputStream.close();
        }
        mDataInputStream = null;
        mDataOutputStream = null;
        mSocket = null;
    }

    @Nullable
    public DataInputStream getDataInputStream() {
        return mDataInputStream;
    }

    @Nullable
    public DataOutputStream getDataOutputStream() {
        return mDataOutputStream;
    }
}
