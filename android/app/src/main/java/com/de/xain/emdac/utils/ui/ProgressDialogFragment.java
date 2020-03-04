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
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringRes;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.de.xain.emdac.R;

public class ProgressDialogFragment extends DialogFragment {

    public interface ProgressDialogListener {
        /**
         * Callback method called when user taps on "Cancel" button
         *
         * @param dialog Dialog which user has interacted with.
         */
        void onCancel(DialogFragment dialog);
    }

    private ProgressDialogListener mListener;
    private View mContentView;
    private AlertDialog.Builder builder;

    private static final String KEY_MESSAGE = "message";
    private static final String KEY_CANCEL_BUTTON_TEXT_ID = "cancelButtonTextId";

    @NonNull
    public static ProgressDialogFragment newInstance(String message, @StringRes int cancelTextId, ProgressDialogListener listener) {
        ProgressDialogFragment frag =  new ProgressDialogFragment();
        Bundle args = new Bundle();
        // set dialog message and cancel button text id
        args.putString(KEY_MESSAGE, message);
        args.putInt(KEY_CANCEL_BUTTON_TEXT_ID, cancelTextId);
        frag.setArguments(args);
        // set dialog listener
        frag.mListener = listener;
        frag.setCancelable(false);
        return frag;
    }

    @SuppressWarnings("unused")
    public void changeMessage(String newMessage) {
        if (mContentView == null) return;

        TextView messageTextView = mContentView.findViewById(R.id.dialog_progress_message);
        if (messageTextView != null) {
            messageTextView.setText(newMessage);
        }
    }

    @SuppressWarnings("unused")
    public void changeProgressBarVisibility(boolean isVisible) {
        if (mContentView == null) return;

        ProgressBar progressBar = mContentView.findViewById(R.id.dialog_progress_progress_bar);
        if (progressBar != null) {
//            progressBar.setVisibility(isVisible ? View.VISIBLE : View.GONE);
            progressBar.setAlpha(isVisible ? 1 : 0);
        }
    }

    public void changeButtonTitle(@StringRes int newTitle) {
        AlertDialog dialog = (AlertDialog) getDialog();
        if (dialog == null) return;
        Button negativeButton = dialog.getButton(DialogInterface.BUTTON_NEGATIVE);
        negativeButton.setText(newTitle);
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(@Nullable Bundle savedInstanceState) {

        Activity activity = getActivity();
        if (activity == null) {
            return super.onCreateDialog(savedInstanceState);
        }

        Bundle args = getArguments();
        String message = "";
        int cancelButtonResId = 0;
        if (args != null) {
            message = args.getString(KEY_MESSAGE);
            cancelButtonResId = args.getInt(KEY_CANCEL_BUTTON_TEXT_ID, 0);
        }

        LayoutInflater inflater = activity.getLayoutInflater();

        mContentView = inflater.inflate(R.layout.dialog_progress, null);
        ProgressBar progressBar = mContentView.findViewById(R.id.dialog_progress_progress_bar);
        progressBar.setIndeterminate(true);
        TextView messageTextView = mContentView.findViewById(R.id.dialog_progress_message);
        messageTextView.setText(message);

        builder = new AlertDialog.Builder(activity);
        builder.setView(mContentView);
//        builder.setNegativeButton(cancelButtonResId, (dialog, which) -> dialog.cancel());

        return builder.create();
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        if (mListener != null) {
            mListener.onCancel(this);
        }
        super.onCancel(dialog);
    }
}
