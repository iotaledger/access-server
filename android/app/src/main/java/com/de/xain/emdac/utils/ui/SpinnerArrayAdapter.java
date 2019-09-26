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

