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

import android.os.AsyncTask;

import timber.log.Timber;

public class TCPCommunicationTask extends AsyncTask<TCPClient, Void, Void> {

    private final String mMessage;

    public TCPCommunicationTask(String message) {
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


