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

package com.de.xain.emdac.utils.ui;

import android.content.Context;
import android.os.Build;
import android.support.annotation.ArrayRes;
import android.support.annotation.LayoutRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.de.xain.emdac.R;

import java.util.Arrays;

public class SpinnerArrayAdapter<T> extends ArrayAdapter<T> {

    public SpinnerArrayAdapter(Context context, T[] objects) {
        super(context, R.layout.spinner_item, objects);
        setDropDownViewResource(R.layout.spinner_item_dropdown);
    }

    @SuppressWarnings("unused")
    public SpinnerArrayAdapter(Context context, int resource, T[] objects) {
        super(context, resource, objects);
        setDropDownViewResource(R.layout.spinner_item_dropdown);
    }

    public static @NonNull ArrayAdapter<CharSequence> createFromResource(@NonNull Context context,
                                                                         @ArrayRes int textArrayResId) {
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(context, textArrayResId,  R.layout.spinner_item);
        adapter.setDropDownViewResource(R.layout.spinner_item_dropdown);
        return adapter;
    }
}

