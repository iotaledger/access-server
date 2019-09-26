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

/**
 * Created by choiintack on 2017. 1. 31..
 */

public class RecursiveItemSet {
    int depth;
    boolean expanded;
    Object item;

    public RecursiveItemSet(Object item, boolean expanded, int depth) {
        this.item = item;
        this.expanded = expanded;
        this.depth = depth;
    }

    public int getDepth() {
        return depth;
    }
    public boolean isExpanded() {
        return expanded;
    }
    public Object getItem() {
        return item;
    }

}
