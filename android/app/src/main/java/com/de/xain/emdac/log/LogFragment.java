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
