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
