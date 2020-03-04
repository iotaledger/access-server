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

package com.de.xain.emdac;

import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.support.annotation.CallSuper;
import android.support.annotation.Nullable;
import android.support.annotation.StringRes;
import android.support.design.widget.Snackbar;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;

import com.de.xain.emdac.utils.ui.DialogFragmentUtil;
import com.de.xain.emdac.utils.ui.DialogFragmentUtil.AlertDialogFragment.AlertDialogListener;
import com.de.xain.emdac.utils.ui.ProgressDialogFragment;
import com.de.xain.emdac.utils.ui.UiUtils;

import timber.log.Timber;

public class BaseFragment extends Fragment {

    private final String FRAGMENT_TAG_PROGRESS_DIALOG = "com.de.xain.emdac.progress_dialog_tag";
    private final String FRAGMENT_TAG_MESSAGE_DIALOG = "com.de.xain.emdac.message_dialog";

    private Snackbar mSnackbar;

    protected void showSnackbar(String message) {
        showSnackbar(message, null);
    }

    @SuppressWarnings("SameParameterValue")
    protected void showSnackbar(String message, View.OnClickListener listener) {
        View fragmentView = getView();
        if (fragmentView == null) return;

        try {
            mSnackbar = Snackbar.make(fragmentView, message, Snackbar.LENGTH_LONG);
            mSnackbar.setAction(android.R.string.ok, v -> {
                mSnackbar.dismiss();
                if (listener != null) {
                    listener.onClick(v);
                }
            });
            mSnackbar.setActionTextColor(Color.GREEN);
            mSnackbar.show();
            Timber.d("Shown snackbar with message: %s", message);
        } catch (Exception e) {
            Timber.e(e);
        }
    }

    @Override
    @CallSuper
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        super.onCreateOptionsMenu(menu, inflater);

        for (int i = 0, n = menu.size(); i < n; i++) {
            MenuItem item = menu.getItem(i);
            Drawable icon = item.getIcon();
            if (icon == null) continue;

            icon = DrawableCompat.wrap(icon);
            DrawableCompat.setTint(icon, UiUtils.getColorFromAttr(getContext(), R.attr.action_menu_icon_color, Color.GRAY));
        }
    }


    protected void showProgress(@Nullable String message, @Nullable ProgressDialogFragment.ProgressDialogListener listener) {
        DialogFragmentUtil.showDialog(DialogFragmentUtil.createProgressDialog(message, android.R.string.cancel, listener),
                getChildFragmentManager(),
                FRAGMENT_TAG_PROGRESS_DIALOG);
    }

    protected void showProgress(@StringRes int message) {
        DialogFragmentUtil
                .showDialog(DialogFragmentUtil.createProgressDialog(getString(message), android.R.string.cancel, null),
                        getChildFragmentManager(),
                        FRAGMENT_TAG_PROGRESS_DIALOG);
    }

    protected void hideProgress() {
        getChildFragmentManager().executePendingTransactions();
        ProgressDialogFragment progressFragment = (ProgressDialogFragment) getChildFragmentManager().findFragmentByTag(FRAGMENT_TAG_PROGRESS_DIALOG);
        if (progressFragment != null) {
            progressFragment.dismiss();
        }
    }

    protected void showDialogMessage(String message) {
        DialogFragment dialog = DialogFragmentUtil.createAlertDialog(message);
        DialogFragmentUtil.showDialog(dialog, getChildFragmentManager(), FRAGMENT_TAG_MESSAGE_DIALOG);
    }

    protected void showDialogMessage(String message, AlertDialogListener listener) {
        DialogFragment dialog = DialogFragmentUtil.createAlertDialog(message, android.R.string.yes, android.R.string.no, listener);
        DialogFragmentUtil.showDialog(dialog, getChildFragmentManager(), FRAGMENT_TAG_MESSAGE_DIALOG);
    }

    protected void hideDialogMessage() {
        Fragment dialog = getChildFragmentManager().findFragmentByTag(FRAGMENT_TAG_MESSAGE_DIALOG);
        if (dialog instanceof DialogFragment) {
            ((DialogFragment) dialog).dismiss();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mSnackbar != null) {
            mSnackbar.dismiss();
            mSnackbar = null;
        }
    }

    /**
     * Removes fragment from back stack.
     */
    protected void finish() {
        FragmentManager fragmentManager = getFragmentManager();
        if (fragmentManager != null && fragmentManager.getBackStackEntryCount() > 0) {

            fragmentManager.popBackStack();
        }
    }
}
