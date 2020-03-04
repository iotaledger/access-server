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
