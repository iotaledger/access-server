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

import android.support.annotation.Keep;

import com.de.xain.emdac.api.tcp.TCPSocketObject;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import timber.log.Timber;

/**
 * API for the TCP communication
 */
public class APILibDacAuthNative {

    /**
     * Sends data through the socket
     *
     * @param sock the socket used for communication
     * @param data data that will be sent
     * @param len  length of the sent data
     * @return number of bytes sent
     */
    @Keep
    @SuppressWarnings("unused")
    public int SendData(TCPSocketObject sock, byte[] data, short len) {
        if (sock == null) {
            Timber.d("TCPSocketObject in SendData is null");
            return 0;
        }

        DataOutputStream outputStream = sock.getDataOutputStream();
        if (outputStream == null) {
            Timber.d("DataOutputStream in SendData is null");
            return 0;
        }

        try {
            outputStream.write(data, 0, len);
            outputStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
            return 0;
        }

        return len;
    }

    /**
     * Receives data from the socket
     *
     * @param sock socket used for communication
     * @param data data read from the socket
     * @param len  length of the data read
     * @return number of bytes read
     */
    @Keep
    @SuppressWarnings("unused")
    public int ReceiveData(TCPSocketObject sock, byte[] data, short len) {
        if (sock == null) {
            Timber.d("TCPSOcketObject in ReceiveData is null");
            return 0;
        }

        DataInputStream dataInputStream = sock.getDataInputStream();

        if (dataInputStream == null) {
            Timber.d("DataInputStream in ReceiveData is null");
            return 0;
        }
        try {
            int bytesRead = dataInputStream.read(data, 0, len);
            Timber.d("ReceiveData len %d", bytesRead);
        } catch (Exception e) {
            Timber.d(e);
            return 0;
        }

        return len;
    }


    /**
     * Native methods that are implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int dacInitClient(long[] session, TCPSocketObject socketObj);

    public native int dacAuthenticate(long[] session);

    public native int dacSetOption(long[] session, String key, byte[] data);

    public native int dacSend(long[] session, byte[] data, short length);

    public native int dacReceive(long[] session, byte[] data, short length);

    public native int dacRelease(long[] session);
}
