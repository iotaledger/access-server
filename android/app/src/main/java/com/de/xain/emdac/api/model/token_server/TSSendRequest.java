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

package com.de.xain.emdac.api.model.token_server;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

public class TSSendRequest implements Serializable {

    @SerializedName("sender")
    @Expose
    private String mSender;

    @SerializedName("receiver")
    @Expose
    private String mReceiver;

    @SerializedName("amount")
    @Expose
    private String amount;

    @SerializedName("priority")
    @Expose
    private int priority;

    public TSSendRequest(String sender, String receiver, String amount, int priority) {
        mSender = sender;
        mReceiver = receiver;
        this.amount = amount;
        this.priority = priority;
    }
}
