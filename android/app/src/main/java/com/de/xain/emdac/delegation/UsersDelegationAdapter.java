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
