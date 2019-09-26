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

import android.arch.lifecycle.ViewModelProviders;
import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.widget.LinearLayoutManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.databinding.FragmentDelegationPreviewStructuredBinding;
import com.de.xain.emdac.di.Injectable;
import com.de.xain.emdac.utils.ui.ViewLifecycleFragment;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import org.json.JSONException;
import org.json.JSONObject;

import javax.inject.Inject;

public class DelegationPreviewStructuredFragment extends ViewLifecycleFragment implements Injectable {

    private static String POLICY = "com.de.xain.emdac.delegation.policy";

    @Inject
    public Gson mGson;

    FragmentDelegationPreviewStructuredBinding mBinding;
    DelegationPreviewViewModel mViewModel;

    JsonAdapter mJsonAdapter;

    private Policy mPolicy;

    public static DelegationPreviewStructuredFragment newInstance(Policy policy) {
        Bundle args = new Bundle();
//        args.putString(POLICY, new GsonBuilder().create().toJson(policy));
        args.putSerializable(POLICY, policy);
        DelegationPreviewStructuredFragment fragment = new DelegationPreviewStructuredFragment();
        fragment.setArguments(args);
        return fragment;
    }

    private void extractArguments() {
        Bundle args = getArguments();
        if (args == null) return;
//        String policyJson = args.getString(POLICY);
//        try {
//            mPolicy = mGson.fromJson(policyJson, Policy.class);
//        } catch (JsonSyntaxException e) {
//            e.printStackTrace();
//        }
        mPolicy = (Policy) args.getSerializable(POLICY);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mBinding = DataBindingUtil.inflate(inflater, R.layout.fragment_delegation_preview_structured, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        extractArguments();

        mViewModel = ViewModelProviders.of(this).get(DelegationPreviewViewModel.class);

//        mBinding.
//        mBinding.textView.setText(new GsonBuilder().setPrettyPrinting().create().toJson(mPolicy));


//        GroupAdapter adapter = new GroupAdapter();
        try {
            JSONObject json = new JSONObject(new GsonBuilder().setPrettyPrinting().create().toJson(mPolicy));
            mJsonAdapter = new JsonAdapter(JsonElement.makeList(json));
            mBinding.recyclerView.setAdapter(mJsonAdapter);
            mBinding.recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        } catch (JSONException e) {
            e.printStackTrace();
        }



    }
}
