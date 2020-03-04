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
