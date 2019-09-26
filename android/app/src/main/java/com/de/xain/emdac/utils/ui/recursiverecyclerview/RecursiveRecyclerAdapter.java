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

package com.de.xain.emdac.utils.ui.recursiverecyclerview;

import android.support.v7.widget.RecyclerView;
import android.view.View;

import java.util.ArrayList;
import java.util.List;

import androidx.annotation.CallSuper;

public abstract class RecursiveRecyclerAdapter<VH extends RecyclerView.ViewHolder> extends RecyclerView.Adapter<VH> {

    private List<RecursiveItemSet> mItems = new ArrayList<>();

    public void setItems(List items) {
        for (Object item : items) {
            mItems.add(new RecursiveItemSet(
                    item, false, 0
            ));
        }
    }

    public Object getItem(int position) {
        return mItems.get(position).getItem();
    }

    public boolean isExpended(int position) {
        return mItems.get(position).expanded;
    }

    public int getDepth(int position) {
        return mItems.get(position).depth;
    }

    @Override
    @CallSuper
    public void onBindViewHolder(VH holder, final int position) {
        if (getItem(position) instanceof RecursiveItem) {
            final RecursiveItem item = (RecursiveItem) getItem(position);
            final Object _item = mItems.get(position);
            holder.itemView.setOnClickListener(v -> {
                if (expanded(_item)) {
                    removeItems(_item, item.getChildren());
                } else {
                    addItems(_item, item.getChildren());
                }
            });
        }
    }

    @Override
    public int getItemCount() {
        return mItems.size();
    }

    @Override
    public int getItemViewType(int position) {
        return mItems.get(position).getDepth();
    }

    private boolean expanded(Object item) {
        int position =  mItems.indexOf(item);
        return mItems.get(position).isExpanded();
    }

    public void addItems(Object parent, List items) {
        int position = mItems.indexOf(parent);
        for (int i = 0; i < items.size(); i ++) {
            Object item = items.get(i);
            mItems.add(position + 1 + i, new RecursiveItemSet(
                    item, false, mItems.get(position).getDepth() + 1
            ));
        }
        mItems.get(position).expanded = true;
        notifyItemRangeInserted(position + 1, items.size());
        notifyItemChanged(position);
    }

    public void removeItems(Object parent, List items) {
        int position = mItems.indexOf(parent);
        for (int i = 0; i < items.size(); i ++) {
            if (getItem(position + 1) instanceof RecursiveItem) {
                if (expanded(mItems.get(position + 1))) {
                    RecursiveItem item = (RecursiveItem) getItem(position + 1);
                    removeItems(mItems.get(position + 1), item.getChildren());
                }
            }
            mItems.remove(position + 1);
        }
        mItems.get(position).expanded = false;
        notifyItemRangeRemoved(position  + 1, items.size());
        notifyItemChanged(position);
    }
    
    public void clearItems() {
        if (mItems != null) {
            mItems.clear();
        }
    }

}
