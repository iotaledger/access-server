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

package com.de.xain.emdac.main.ui;

import android.databinding.DataBindingUtil;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.CheckBox;

import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.VehicleInfoListItemBinding;
import com.de.xain.emdac.main.model.VehicleInfo;

import java.util.List;

public class VehicleInfoAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    class ViewHolder extends RecyclerView.ViewHolder {
        private CheckBox mCheckBox;
        private VehicleInfo mVehicleInfo;

        ViewHolder(VehicleInfoListItemBinding binding) {
            super(binding.getRoot());
            mCheckBox = binding.checkboxVehicleInfoListItem;
            mCheckBox.setOnCheckedChangeListener((buttonView, isChecked) -> {
                if (mVehicleInfo == null) {
                    return;
                }
                mVehicleInfo.setSelected(isChecked);
            });
        }

        public void bind(VehicleInfo vehicleInfo) {
            mVehicleInfo = vehicleInfo;
            mCheckBox.setChecked(vehicleInfo.isSelected());
            mCheckBox.setText(vehicleInfo.getName());
        }
    }

    private List<VehicleInfo> mDataSet;

    public VehicleInfoAdapter(List<VehicleInfo> dataSet) {
        mDataSet = dataSet;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        VehicleInfoListItemBinding binding = DataBindingUtil.inflate(LayoutInflater.from(viewGroup.getContext()),
                R.layout.vehicle_info_list_item,
                viewGroup,
                false);
        return new VehicleInfoAdapter.ViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder viewHolder, int i) {
        if (viewHolder instanceof VehicleInfoAdapter.ViewHolder) {
            ((VehicleInfoAdapter.ViewHolder) viewHolder).bind(mDataSet.get(i));
        }
    }

    @Override
    public int getItemCount() {
        return mDataSet.size();
    }
}
