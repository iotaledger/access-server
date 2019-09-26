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

package com.de.xain.emdac.main.model;

import java.io.Serializable;

import io.reactivex.annotations.Nullable;

public class VehicleInfo implements Serializable {
    private boolean mSelected;
    private final String mName;
    private final String mId;

    public VehicleInfo(String id, String name) {
        this(id, name, false);
    }

    public VehicleInfo(String id, String name, boolean selected) {
        mId = id;
        mName = name;
        mSelected = selected;
    }

    public boolean isSelected() {
        return mSelected;
    }

    public void setSelected(boolean selected) {
        mSelected = selected;
    }

    public String getName() {
        return mName;
    }

    public String getId() {
        return mId;
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj instanceof VehicleInfo) {
            return mId.equals(((VehicleInfo) obj).mId);
        } else {
            return false;
        }
    }

    @Override
    public VehicleInfo clone() {
        VehicleInfo vehicleInfo;
        try {
            vehicleInfo = (VehicleInfo) super.clone();
        } catch (Exception ignored) {
            vehicleInfo = new VehicleInfo(mId, mName, mSelected);
        }
        return vehicleInfo;
    }
}
