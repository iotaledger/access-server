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

package com.de.xain.emdac.log;

import android.content.Context;
import android.content.res.TypedArray;
import android.databinding.DataBindingUtil;
import android.support.annotation.NonNull;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;

import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.SpinnerItemWithIconBinding;

/**
 * Custom spinner adapter for inflating spinner items with text and icon
 */
public class CustomSpinnerAdapter extends ArrayAdapter<String> {

    private Context mCtx;
    // list of options
    private String[] mOptions;
    // list of options icons
    private TypedArray mImages;

    CustomSpinnerAdapter(Context context, String[] options,
                         TypedArray images) {
        super(context, R.layout.spinner_item_with_icon, R.id.text_spinner_option, options);
        this.mCtx = context;
        this.mOptions = options;
        this.mImages = images;
    }

    @Override
    public View getDropDownView(int position, View convertView, @NonNull ViewGroup parent) {
        return getCustomView(position, convertView, parent);
    }

    @Override
    @NonNull
    public View getView(int position, View convertView, @NonNull ViewGroup parent) {
        return getCustomView(position, convertView, parent);
    }

    private View getCustomView(int position, View convertView, ViewGroup parent) {

        LayoutInflater inflater = (LayoutInflater) mCtx.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (inflater != null) {
            // inflate custom layout and set corresponding data
            SpinnerItemWithIconBinding binding = DataBindingUtil.inflate(inflater, R.layout.spinner_item_with_icon, parent, false);
            binding.textSpinnerOption.setText(mOptions[position]);
            binding.icon.setImageDrawable(mImages.getDrawable(position));
            return binding.getRoot();
        } else {
            return convertView;
        }
    }
}
