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

import android.support.annotation.StringRes;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.policy.PolicyAttributeList;

public enum RuleSatisfyType {
    ONE_OR_MORE, // if there are multiple rules, one or more should be sattisfied
    EVERY; // if there are multiple rules, every rule should be sattisfied

    public static RuleSatisfyType[] getAllValues() {
        return new RuleSatisfyType[]{
                ONE_OR_MORE,
                EVERY
        };
    }

    @StringRes
    public int getNameResId() {
        switch (this) {
            case ONE_OR_MORE:
                return R.string.rule_satisfy_type_one_or_more;
            case EVERY:
                return R.string.rule_satisfy_type_every;
        }
        return 0;
    }

    public PolicyAttributeList.Operation toOperation() {
        switch (this) {
            case ONE_OR_MORE:
                return PolicyAttributeList.Operation.OR;
            case EVERY:
            default:
                return PolicyAttributeList.Operation.AND;
        }
    }
}
