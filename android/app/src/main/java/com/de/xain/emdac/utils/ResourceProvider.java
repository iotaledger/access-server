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

package com.de.xain.emdac.utils;

import android.content.Context;
import android.content.res.Resources;

public class ResourceProvider {
    private Context mContext;

    public ResourceProvider(Context context) {
        this.mContext = context;
    }

    public String getString(int resId) {
        return mContext.getString(resId);
    }

    public String getString(int resId, String value) {
        return mContext.getString(resId, value);
    }

    public Resources getResources() {
        return mContext.getResources();
    }
}
