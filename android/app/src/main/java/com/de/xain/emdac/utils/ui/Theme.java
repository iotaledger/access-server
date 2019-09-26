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

import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringRes;
import android.support.annotation.StyleRes;

public class Theme {
    private final int mThemeId;
    private final Integer mNameId;
    private final Integer mTitleId;
    private final Integer mLogoId;
    private final Integer mLargeLogoId;

    public Theme(int themeId, @NonNull Integer nameId, @Nullable Integer titleId, @Nullable Integer logoId, @Nullable Integer largeLogoId) {
        mThemeId = themeId;
        mNameId = nameId;
        mTitleId = titleId;
        mLogoId = logoId;
        mLargeLogoId = largeLogoId;
    }

    @StyleRes
    public int getThemeId() {
        return mThemeId;
    }

    @StringRes
    public Integer getNameId() {
        return mNameId;
    }

    @Nullable
    public Integer getTitleId() {
        return mTitleId;
    }

    @Nullable
    public Integer getLogoId() {
        return mLogoId;
    }

    @Nullable
    public Integer getLargeLogoId() {
        return mLargeLogoId;
    }

}
