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
