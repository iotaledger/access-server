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
import android.content.res.TypedArray;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.support.annotation.AttrRes;
import android.support.annotation.ColorInt;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.util.TypedValue;
import android.widget.ImageView;

import com.de.xain.emdac.R;

public class UiUtils {

    @ColorInt
    public static int getColorFromAttr(@Nullable Context context, @AttrRes int attr, @ColorInt int failColor) {
        int color = failColor;
        if (context == null) return color;

        TypedValue typedValue = new TypedValue();

        int[] colorAttr = new int[]{attr};
        TypedArray a = context.obtainStyledAttributes(typedValue.data, colorAttr);

        color = a.getColor(0, failColor);
        a.recycle();
        return color;
    }

    @Nullable
    public static Drawable getCompatDrawable(@NonNull Context context, @DrawableRes int resId) {
        Drawable drawable = ContextCompat.getDrawable(context, resId);
        if (drawable != null) {
            drawable = DrawableCompat.wrap(drawable);
        }
        return drawable;
    }

    public static void setImageWithTint(ImageView imageView, @DrawableRes int resId, @AttrRes int attr, @ColorInt int failColor) {
        Drawable image = UiUtils.getCompatDrawable(imageView.getContext(), resId);
        if (image != null) {
            image = image.mutate();
            image.setTint(UiUtils.getColorFromAttr(imageView.getContext(), attr, failColor));
            imageView.setBackground(image);
        }
    }
}
