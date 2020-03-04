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
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.Command;
import com.de.xain.emdac.databinding.CommandListItemBinding;
import com.de.xain.emdac.utils.ui.UiUtils;

import java.util.List;

/**
 * Adapter for the command list
 */

public class CommandsAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private final int FADE_DURATION = 200; // in miliseconds
    private List<Command> mDataSet;
    private OnSelectedCommandListener mOnSelectedCommandListener;

    // Provide a suitable constructor (depends on the kind of dataset)
    CommandsAdapter(List<Command> dataSet, OnSelectedCommandListener onSelectedCommandListener) {
        mDataSet = dataSet;
        mOnSelectedCommandListener = onSelectedCommandListener;
    }

    // Create new views (invoked by the layout manager)
    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        CommandListItemBinding binding = DataBindingUtil.inflate(LayoutInflater.from(parent.getContext()),
                R.layout.command_list_item,
                parent,
                false);
        return new CommandViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder viewHolder, int position) {
        CommandViewHolder commandHolder = (CommandViewHolder) viewHolder;
        configureViewForCommand(commandHolder, position);
    }


    private void configureViewForCommand(@NonNull CommandViewHolder holder, int position) {
        Command command = mDataSet.get(position);

        holder.mCommandButton.setText(command.getActionName());
        holder.mCommandName.setText(command.getHeaderName());


        UiUtils.setImageWithTint(holder.mImageHeader, command.getHeaderImageResId(), R.attr.command_list_item_header_image_tint, Color.WHITE);

        UiUtils.setImageWithTint(holder.mImageStatus, command.getStatusImageResId(), R.attr.command_list_item_status_image_tint, Color.WHITE);

//        holder.mImageStatus.setBackgroundResource(command.getStatusImageResId());
        holder.mCommandButton.setOnClickListener(v -> mOnSelectedCommandListener.onCommandSelected(command));
        holder.mDeleteButton.setVisibility(command.isDeletable() ? View.VISIBLE : View.INVISIBLE);
        holder.mDeleteButton.setOnClickListener(v -> mOnSelectedCommandListener.onCommandDeleted(command));
        holder.mIsPaidTextView.setVisibility(command.getActiveAction().isPaid() ? View.GONE : View.VISIBLE);
    }

    // Return the size of your data set (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataSet.size();
    }

    /**
     * Helper interface for detecting interactions with the list items
     */
    public interface OnSelectedCommandListener {

        void onMicrophoneClick();

        void onCommandSelected(Command command);

        void onCommandDeleted(Command command);
    }

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder
    static class CommandViewHolder extends RecyclerView.ViewHolder {
        ImageButton mDeleteButton;
        TextView mCommandName;
        ImageView mImageHeader;
        ImageView mImageStatus;
        Button mCommandButton;
        TextView mIsPaidTextView;

        CommandViewHolder(CommandListItemBinding binding) {
            super(binding.getRoot());
            mDeleteButton = binding.buttonDelete;
            mCommandName = binding.textCommandName;
            mImageHeader = binding.imgHeader;
            mImageStatus = binding.imgStatus;
            mCommandButton = binding.buttonCommand;
            mIsPaidTextView = binding.isPaidTextView;
        }
    }

}
