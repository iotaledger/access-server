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
