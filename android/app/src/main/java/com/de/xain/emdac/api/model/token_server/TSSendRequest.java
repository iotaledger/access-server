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
