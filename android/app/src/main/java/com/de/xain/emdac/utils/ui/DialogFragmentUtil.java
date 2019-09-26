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

package com.de.xain.emdac.utils.ui;

import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringRes;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentManager;
import android.support.v7.app.AlertDialog;
import android.widget.Button;

import com.de.xain.emdac.R;

/**
 * Helper class for creating and displaying dialogs
 */
public class DialogFragmentUtil {

    public static DialogFragment createAlertDialog(String message) {
        return AlertDialogFragment.newInstance(message, null);
    }

    public static DialogFragment createAlertDialog(String message, int positiveTextId, int negativeTextId, AlertDialogFragment.AlertDialogListener listener) {
        return AlertDialogFragment.newInstance(message, positiveTextId, negativeTextId, listener);
    }

    public static ProgressDialogFragment createProgressDialog(String message, @StringRes int cancelTextRes,@Nullable ProgressDialogFragment.ProgressDialogListener listener) {
        return ProgressDialogFragment.newInstance(message, cancelTextRes, listener);
    }

    public static ListDialogFragment createListDialog(CharSequence[] items, boolean[] selectedItems, ListDialogFragment.Listener listener) {
        return ListDialogFragment.newInstance(items, selectedItems, listener);
    }

    /**
     * AlertDialog class which uses {@link DialogFragment}
     */
    public static class AlertDialogFragment extends DialogFragment {

        @Override
        public void onCreate(@Nullable Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
        }

        /**
         * Helper interface for detecting button clicks on dialog
         */
        public interface AlertDialogListener {
            void onPositiveClick(DialogFragment dialog);
        }

        private static AlertDialogListener mListener;

        private static final String KEY_MESSAGE = "message";
        private static final String KEY_POSITIVE_BUTTON_TEXT_ID = "positiveButtonTextId";
        private static final String KEY_NEGATIVE_BUTTON_TEXT_ID = "negativeButtonTextId";

        /**
         * Provides a new instance of {@link AlertDialogFragment}
         *
         * @param message              message of the dialog
         * @param positiveButtonTextId positive button text id
         * @param negativeButtonTextId negative button text id
         * @param listener             listener for click events
         * @return a {@link AlertDialogFragment}
         */
        public static AlertDialogFragment newInstance(String message, int positiveButtonTextId, int negativeButtonTextId, AlertDialogListener listener) {
            AlertDialogFragment frag = newInstance(message, listener);

            Bundle args = frag.getArguments();
            if (args != null) {
                args.putInt(KEY_POSITIVE_BUTTON_TEXT_ID, positiveButtonTextId);
                args.putInt(KEY_NEGATIVE_BUTTON_TEXT_ID, negativeButtonTextId);
            }
            frag.setArguments(args);
            return frag;
        }

        /**
         * Provides a new instance of {@link AlertDialogFragment}
         *
         * @param message  message of the dialog
         * @param listener listener for click events
         * @return a {@link AlertDialogFragment}
         */
        public static AlertDialogFragment newInstance(String message, AlertDialogListener listener) {
            AlertDialogFragment frag = new AlertDialogFragment();
            Bundle args = new Bundle();
            // set dialog message
            args.putString(KEY_MESSAGE, message);
            frag.setArguments(args);
            // set dialog listener
            mListener = listener;
            return frag;
        }

        @Override
        @NonNull
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            Bundle args = getArguments();
            String message = "";
            int positiveTextId = 0;
            int negativeTextId = 0;
            if (args != null) {
                message = getArguments().getString(KEY_MESSAGE);
                positiveTextId = getArguments().getInt(KEY_POSITIVE_BUTTON_TEXT_ID, 0);
                negativeTextId = getArguments().getInt(KEY_NEGATIVE_BUTTON_TEXT_ID, 0);
            }

            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(getActivity());
            dialogBuilder.setMessage(message);
            dialogBuilder.setPositiveButton(negativeTextId == 0 ? android.R.string.ok : positiveTextId, (DialogInterface dialog, int whichButton) -> {

                if (mListener != null) {
                    mListener.onPositiveClick(AlertDialogFragment.this);
                }
                // dismiss dialog
                dialog.dismiss();
            });
            if (negativeTextId != 0) {
                dialogBuilder.setNegativeButton(negativeTextId, (dialog, which) -> dialog.dismiss());
            }

            AlertDialog dialog = dialogBuilder.create();
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
    }

    public static void showDialog(DialogFragment dialog, FragmentManager fragmentManager, String TAG) {
        dialog.show(fragmentManager, TAG);
    }
}
