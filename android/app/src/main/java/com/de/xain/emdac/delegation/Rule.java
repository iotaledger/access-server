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
