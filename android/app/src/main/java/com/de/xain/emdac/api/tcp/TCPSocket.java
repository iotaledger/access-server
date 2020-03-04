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

import android.support.annotation.NonNull;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;

public class TCPSocket extends Socket {

    private DataInputStream dataInputStream;
    private DataOutputStream dataOutputStream;

    TCPSocket(InetAddress address, int port) throws IOException {
        super(address, port);

        // set timeout to be 10 seconds
        setSoTimeout(10000);

        //sends the message to the server
        dataOutputStream = new DataOutputStream(getOutputStream());

        //receives the message which the server sends back
        dataInputStream = new DataInputStream(getInputStream());
    }

    @NonNull
    public DataInputStream getDataInputStream() {
        return dataInputStream;
    }

    @NonNull
    public DataOutputStream getDataOutputStream() {
        return dataOutputStream;
    }
}