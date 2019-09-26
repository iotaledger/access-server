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

package com.de.xain.emdac.delegation;

import android.content.res.Resources;
import android.support.annotation.StringRes;

import com.de.xain.emdac.R;

public enum RuleLimitation {
    TIME,
    LOCATION,
    SPEED_LIMIT;

    @StringRes
    public int getNameResId() {
        switch (this) {
            case TIME:
                return R.string.rule_limitation_time;
            case LOCATION:
                return R.string.rule_limitation_location;
            case SPEED_LIMIT:
                return R.string.rule_limitation_speed_limit;
        }
        return 0;
    }

    public static RuleLimitation[] getAllValues() {
        return new RuleLimitation[]{
                TIME,
                LOCATION,
                SPEED_LIMIT};
    }

    public static String[] getAllStringValues(Resources resources) {
        return new String[]{
                resources.getString(RuleLimitation.TIME.getNameResId()),
                resources.getString(RuleLimitation.LOCATION.getNameResId()),
                resources.getString(RuleLimitation.SPEED_LIMIT.getNameResId())};
    }

    public enum TimeType {
        FROM,
        UNTIL
    }
}
