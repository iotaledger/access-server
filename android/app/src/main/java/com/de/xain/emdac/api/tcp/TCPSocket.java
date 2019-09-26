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