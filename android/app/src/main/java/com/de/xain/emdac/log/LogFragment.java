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

package com.de.xain.emdac.log;

import android.databinding.DataBindingUtil;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import com.de.xain.emdac.BaseFragment;
import com.de.xain.emdac.R;
import com.de.xain.emdac.databinding.FragmentLogBinding;
import com.de.xain.emdac.utils.ui.SpinnerArrayAdapter;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class LogFragment extends BaseFragment {

    public static LogFragment newInstance() {
        return new LogFragment();
    }

    private Spinner mLogSpinner;
    private TextView mLogTextView;
    private ScrollView mScrollView;

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        FragmentLogBinding binding = DataBindingUtil.inflate(inflater, R.layout.fragment_log, container, false);
        mLogSpinner = binding.spinnerLog;
        mLogTextView = binding.textLog;
        mScrollView = binding.scrollView;
        return binding.getRoot();
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        ArrayAdapter<CharSequence> adapter = new SpinnerArrayAdapter<>(getActivity(), getResources().getStringArray(R.array.log_types));
        mLogSpinner.setAdapter(adapter);
        // start gathering application logs and put them into a TextView
        try {
            Process process = Runtime.getRuntime().exec("logcat -d");
            BufferedReader bufferedReader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            StringBuilder log = new StringBuilder();
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                line += "\n\n";
                log.append(line);
            }
            mLogTextView.setText(log.toString());
        } catch (IOException e) {
            e.printStackTrace();
        }
        // scroll to the end of the log as it is the most recent
        mScrollView.post(() -> mScrollView.fullScroll(ScrollView.FOCUS_DOWN));
    }
}
