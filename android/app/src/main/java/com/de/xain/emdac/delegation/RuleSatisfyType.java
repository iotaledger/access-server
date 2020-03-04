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
