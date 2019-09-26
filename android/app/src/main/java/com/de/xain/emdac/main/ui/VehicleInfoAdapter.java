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
