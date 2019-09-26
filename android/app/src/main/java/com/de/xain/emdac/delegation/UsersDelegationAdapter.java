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

import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.Button;

import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.UserDelegateListItemBinding;

import java.util.List;

public class UsersDelegationAdapter extends RecyclerView.Adapter<UsersDelegationAdapter.ViewHolder> {

    private List<UserPublic> mUserPublicList;
    private OnItemClickListener mListener;

    static class ViewHolder extends RecyclerView.ViewHolder {

        private Button mButton;

        ViewHolder(UserDelegateListItemBinding binding) {
            super(binding.getRoot());
            mButton = binding.button;
            itemView.setBackgroundColor(Color.RED);
        }

    }

    public UsersDelegationAdapter(List<UserPublic> userPublicList, OnItemClickListener listener) {
        mUserPublicList = userPublicList;
        mListener = listener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        UserDelegateListItemBinding binding = DataBindingUtil.inflate(LayoutInflater.from(viewGroup.getContext()), R.layout.user_delegate_list_item, viewGroup, false);
        return new ViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder viewHolder, int i) {
        if (i == mUserPublicList.size()) {
            // add new user cell
            viewHolder.mButton.setText("Add new user");
            viewHolder.mButton.setOnClickListener(__ -> mListener.onAddNewUserClick());
        } else {
            // added user cell
            viewHolder.mButton.setText(mUserPublicList.get(i).getUsername());
            viewHolder.mButton.setOnClickListener(__ -> mListener.onUserClick(mUserPublicList.get(i)));
        }
    }

    @Override
    public int getItemCount() {
        return mUserPublicList.size() + 1;
    }

    public interface OnItemClickListener {
        void onUserClick(UserPublic userPublic);
        void onAddNewUserClick();
    }
}
