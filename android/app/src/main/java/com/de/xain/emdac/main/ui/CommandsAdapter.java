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
