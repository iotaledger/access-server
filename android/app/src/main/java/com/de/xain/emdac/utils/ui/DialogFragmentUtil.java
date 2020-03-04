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
