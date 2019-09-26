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

package com.de.xain.emdac.delegation.preview;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;

import com.de.xain.emdac.api.model.policy.Policy;

public class DelegationPreviewPagerAdapter extends FragmentStatePagerAdapter {

    private Policy mPolicy;

    public DelegationPreviewPagerAdapter(FragmentManager fm, Policy policy) {
        super(fm);
        mPolicy = policy;
    }

    @Override
    public Fragment getItem(int position) {
        switch (position) {
            case 0:
                return DelegationPreviewStructuredFragment.newInstance(mPolicy);
            case 1:
                return DelegationPreviewJsonFragment.newInstance(mPolicy);
            default:
                return null;
        }
    }

    @Override
    public int getCount() {
        return 2;
    }
}
