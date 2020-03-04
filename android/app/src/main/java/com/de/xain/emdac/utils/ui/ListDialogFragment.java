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

package com.de.xain.emdac.utils.ui;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.DialogFragment;
import android.widget.Button;

public class ListDialogFragment extends DialogFragment {

    private static String ITEMS = "com.de.xain.emdac.utils.ui.ListDialogFragment.items";
    private static String SELECTED_ITEMS = "com.de.xain.emdac.utils.ui.ListDialogFragment.selected_items";

    private Listener mListener;
    private CharSequence[] mItems;
    private boolean[] mSelectedItems;

    @NonNull
    public static ListDialogFragment newInstance(CharSequence[] items, boolean[] selectedItems, Listener listener) {
        ListDialogFragment fragment = new ListDialogFragment();
        Bundle args = new Bundle();

        args.putCharSequenceArray(ITEMS, items);
        args.putBooleanArray(SELECTED_ITEMS, selectedItems);
        fragment.setArguments(args);

        fragment.mListener = listener;

        return fragment;
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(@Nullable Bundle savedInstanceState) {
        Activity activity = getActivity();
        Bundle args = getArguments();
        if (activity == null || args == null)
            return super.onCreateDialog(savedInstanceState);

        mItems = args.getCharSequenceArray(ITEMS);
        mSelectedItems = args.getBooleanArray(SELECTED_ITEMS);


        AlertDialog.Builder builder = new AlertDialog.Builder(activity);
        builder.setMultiChoiceItems(mItems, mSelectedItems, (__, which, isChecked) -> mSelectedItems[which] = isChecked);
        builder.setPositiveButton(android.R.string.ok, (__, ___) -> mListener.onSelectionFinish(mSelectedItems));
        builder.setNegativeButton(android.R.string.cancel, null);

        AlertDialog dialog = builder.create();
        dialog.setOnShowListener(dialog1 -> {
            if (dialog1 instanceof AlertDialog) {
                AlertDialog alertDialog = ((AlertDialog) dialog1);
                Button positiveButton = alertDialog.getButton(AlertDialog.BUTTON_POSITIVE);
                if (positiveButton != null) {
                    positiveButton.setTextColor(Color.BLACK);
                }
                Button negativeButton = alertDialog.getButton(AlertDialog.BUTTON_NEGATIVE);
                if (negativeButton != null) {
                    negativeButton.setTextColor(Color.BLACK);
                }
            }
        });
        return dialog;
    }

    public interface Listener {
        void onSelectionFinish(boolean[] selected);
    }
}
