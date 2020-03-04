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

package com.de.xain.emdac.api.model;

import android.support.annotation.Nullable;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

public class TCPResponse<T> implements Serializable {

    @SerializedName("error")
    private Integer error;

    @SerializedName("message")
    private String mMessage;

    @SerializedName("data")
    @Nullable
    private T mData;

    public TCPResponse() {
    }

    public Boolean isSuccessful() {
        if (error != null) {
            return error == 0;
        } else {
            return true;
        }
    }

    public void setError(Integer error) {
        this.error = error;
    }

    @Nullable
    public String getMessage() {
        return mMessage;
    }

    public void setMessage(String message) {
        mMessage = message;
    }

    @Nullable
    public T getData() {
        return mData;
    }

    public void setData(T data) {
        mData = data;
    }

}
