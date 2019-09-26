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

import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.databinding.FragmentDelegationPreviewJsonBinding;
import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.utils.ui.ViewLifecycleFragment;
import com.google.gson.GsonBuilder;

public class DelegationPreviewJsonFragment extends ViewLifecycleFragment {

    private static String POLICY = "com.de.xain.emdac.delegation.policy";

    private FragmentDelegationPreviewJsonBinding mBinding;

    private Policy mPolicy;

    public static DelegationPreviewJsonFragment newInstance(Policy policy) {
        Bundle args = new Bundle();
        args.putSerializable(POLICY, policy);
        DelegationPreviewJsonFragment fragment = new DelegationPreviewJsonFragment();
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
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_delegation_preview_json, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        extractArguments();
        String text = new GsonBuilder().setPrettyPrinting().create().toJson(mPolicy);
        mBinding.textView.setText(text);
    }
}
