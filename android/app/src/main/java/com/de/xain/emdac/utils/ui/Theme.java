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
