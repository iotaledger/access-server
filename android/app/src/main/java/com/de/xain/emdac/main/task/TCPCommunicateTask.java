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

package com.de.xain.emdac.main.task;

import android.os.AsyncTask;

import com.de.xain.emdac.api.tcp.TCPClient;

import timber.log.Timber;

public class TCPCommunicateTask extends AsyncTask<TCPClient, Void, Void> {

    private final String mMessage;

    public TCPCommunicateTask(String message) {
        mMessage = message;
        Timber.d("Sending message: %s", message);
    }

    @Override
    protected Void doInBackground(TCPClient... tcpClients) {
        if (tcpClients.length >= 1) {
            tcpClients[0].sendMessage(mMessage);
        }
        return null;
    }
}
