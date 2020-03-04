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

import android.os.Parcelable;

import androidx.annotation.Nullable;

import com.de.xain.emdac.api.model.policy.PolicyAttribute;

import java.util.UUID;

public abstract class Rule implements Parcelable {

    protected UUID mId = UUID.randomUUID();

    @Override
    public int hashCode() {
        return mId.hashCode();
    }

    public UUID getId() {
        return mId;
    }

    public void setId(UUID mId) {
        this.mId = mId;
    }

    /**
     * Create policy attribute from this rule.
     * @return Created policy attribute.
     */
    abstract public PolicyAttribute toPolicyAttribute();

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj instanceof Rule)
            return mId.equals(((Rule) obj).mId);
        else
            return false;
    }
}
