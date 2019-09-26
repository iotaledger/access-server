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

import android.annotation.SuppressLint;
import android.databinding.DataBindingUtil;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.ItemJsonElementBinding;
import com.de.xain.emdac.utils.ui.DisplayUtil;
import com.de.xain.emdac.utils.ui.recursiverecyclerview.RecursiveRecyclerAdapter;

import java.util.List;

public class JsonAdapter extends RecursiveRecyclerAdapter<JsonAdapter.ViewHolder> {

    public JsonAdapter(List<JsonElement> items) {
        setItems(items);
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        ItemJsonElementBinding binding = DataBindingUtil.inflate(LayoutInflater.from(viewGroup.getContext()), R.layout.item_json_element, viewGroup, false);
        return new ViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        Object item = getItem(position);
        if (item instanceof JsonElement) {
            holder.bind((JsonElement) item, getDepth(position), isExpended(position));
        }
    }

    public class ViewHolder extends RecyclerView.ViewHolder {

        private final TextView mKeyTextView;
        private final TextView mValueTextView;
        private final ImageView mArrowImageView;

        private ViewHolder(ItemJsonElementBinding binding) {
            super(binding.getRoot());
            mKeyTextView = binding.textKey;
            mValueTextView = binding.textValue;
            mArrowImageView = binding.imageArrow;
        }

        @SuppressLint("SetTextI18n")
        private void bind(JsonElement jsonElement, int depth, boolean expended) {
            mKeyTextView.setText(jsonElement.getKey() + ":");
            mValueTextView.setText(jsonElement.getValue());

            mKeyTextView.setPadding(
                    DisplayUtil.convertDensityPixelToPixel(itemView.getContext(), 5)
                            + depth * DisplayUtil.convertDensityPixelToPixel(itemView.getContext(), 20),
                    0, 0, 0);

            if (jsonElement.getChildren().size() == 0)
                mArrowImageView.setVisibility(View.GONE);
            else {
                if (expended)
                    mArrowImageView.setImageResource(R.drawable.ic_arrow_up);
                else
                    mArrowImageView.setImageResource(R.drawable.ic_arrow_down);
                mArrowImageView.setVisibility(View.VISIBLE);
            }


        }
    }
}
