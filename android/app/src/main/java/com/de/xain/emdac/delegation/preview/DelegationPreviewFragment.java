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

package com.de.xain.emdac.delegation.preview;

import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.design.widget.TabLayout;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.databinding.FragmentDelegationPreviewBinding;
import com.de.xain.emdac.utils.ui.UiUtils;
import com.de.xain.emdac.utils.ui.ViewLifecycleFragment;

public class DelegationPreviewFragment extends ViewLifecycleFragment {

    private static String POLICY = "com.de.xain.emdac.delegation.policy";

    private FragmentDelegationPreviewBinding mBinding;
    private Policy mPolicy;

    public static DelegationPreviewFragment newInstance(Policy policy) {
        DelegationPreviewFragment fragment = new DelegationPreviewFragment();
        Bundle args = new Bundle();
        args.putSerializable(POLICY, policy);
        fragment.setArguments(args);
        return fragment;
    }

    private void extractArguments() {
        Bundle args = getArguments();
        if (args == null) return;
        mPolicy = (Policy) args.getSerializable(POLICY);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_delegation_preview, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        extractArguments();

        mBinding.tabLayout.addTab(mBinding.tabLayout.newTab().setText(R.string.structured));
        mBinding.tabLayout.addTab(mBinding.tabLayout.newTab().setText(R.string.json));
        mBinding.tabLayout.setTabGravity(TabLayout.GRAVITY_FILL);
        mBinding.tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                mBinding.pager.setCurrentItem(tab.getPosition());
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {
            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {
            }
        });

        mBinding.pager.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(mBinding.tabLayout));

        DelegationPreviewPagerAdapter adapter = new DelegationPreviewPagerAdapter(getChildFragmentManager(), mPolicy);
        mBinding.pager.setAdapter(adapter);

        mBinding.tabLayout.setTabTextColors(
                UiUtils.getColorFromAttr(mBinding.tabLayout.getContext(), R.attr.nav_drawer_color_item_text_unchecked, Color.GRAY),
                UiUtils.getColorFromAttr(mBinding.tabLayout.getContext(), R.attr.nav_drawer_color_item_text_unchecked, Color.GRAY)
        );
    }
}
