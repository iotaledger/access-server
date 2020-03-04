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

package com.de.xain.emdac.command_editor;

import android.databinding.Bindable;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.android.databinding.library.baseAdapters.BR;
import com.de.xain.emdac.BaseObservableViewModel;
import com.de.xain.emdac.R;
import com.de.xain.emdac.api.model.Command;
import com.de.xain.emdac.api.model.CommandAction;
import com.de.xain.emdac.di.AppSharedPreferences;
import com.de.xain.emdac.utils.ResourceProvider;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.subjects.PublishSubject;


public class CommandEditorViewModel extends BaseObservableViewModel {

    private final String COMMAND_NAME = "com.de.xain.emdac.command_name";
    private final String COMMAND_JSON = "com.de.xain.emdac.command_json";
    private final AppSharedPreferences mPreferences;
    private final ResourceProvider mResourceProvider;
    private final PublishSubject<String> mShowDialogMessage = PublishSubject.create();
    private final PublishSubject<Boolean> mShowLoading = PublishSubject.create();
    private final PublishSubject<Command> mNewCommand = PublishSubject.create();
    private final PublishSubject<String> mSnackbarMessage = PublishSubject.create();

    private String mCommandName;
    private String mCommandJson;

    @Inject
    public CommandEditorViewModel(AppSharedPreferences sharedPreferences, ResourceProvider resourceProvider) {
        mPreferences = sharedPreferences;
        mResourceProvider = resourceProvider;
    }

    public void restoreState(@Nullable Bundle state) {
        if (state == null) return;
        if (state.containsKey(COMMAND_NAME))
            setCommandName(state.getString(COMMAND_NAME));
        if (state.containsKey(COMMAND_JSON))
            setCommandJson(state.getString(COMMAND_JSON));
    }

    public Bundle getStateToSave() {
        Bundle state = new Bundle();
        state.putString(COMMAND_NAME, mCommandName);
        state.putString(COMMAND_JSON, mCommandJson);
        return state;
    }

    public Observable<String> getShowDialogMessage() {
        return mShowDialogMessage;
    }

    public Observable<Boolean> getShowLoading() {
        return mShowLoading;
    }

    public Observable<Command> getNewCommand() {
        return mNewCommand;
    }

    public Observable<String> getSnackbarTextMessage() {
        return mSnackbarMessage;
    }

    @Bindable
    public String getCommandName() {
        return mCommandName;
    }

    public void setCommandName(String commandName) {
        mCommandName = commandName;
        notifyPropertyChanged(BR.commandName);
    }

    @Bindable
    public String getCommandJson() {
        return mCommandJson;
    }

    public void setCommandJson(String commandJson) {
        mCommandJson = commandJson;
        notifyPropertyChanged(BR.commandJson);
    }

    public void createNewCommand() {
        if (!checkIsValid()) return;
        mShowLoading.onNext(true);

        new CountDownTimer(1500, 1500) {
            @Override
            public void onTick(long millisUntilFinished) {
            }

            @Override
            public void onFinish() {
                mShowLoading.onNext(false);

                CommandAction action = new CommandAction();
                action.setAction(mCommandName);
                action.setActionName(mCommandName);
                action.setHeaderName(mCommandName);

                Command newCommand;
                try {
                    newCommand = new Command.Builder()
                            .setActionList(new CommandAction[]{action})
                            .setActiveAction(action)
                            .setState(Command.KnownState.COMMAND_STATE_LOCKED)
                            .setDeletable(true)
                            .create();
                } catch (Command.CommandException e) {
                    e.printStackTrace();
                    return;
                }
                saveNewCommand(newCommand);
                mNewCommand.onNext(newCommand);
                mSnackbarMessage.onNext(mResourceProvider.getString(R.string.msg_command_successfully_created));
                setCommandName(null);
                setCommandJson(null);
            }
        }.start();
    }

    private void saveNewCommand(Command newCommand) {
        List<Command> oldList = mPreferences.getCommandList();
        if (oldList == null)
            oldList = new ArrayList<>();
        oldList.add(newCommand);
        mPreferences.putCommandList(oldList);
    }

    private boolean checkIsValid() {
        if (TextUtils.isEmpty(mCommandName)) {
            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_command_name_empty));
            return false;
        }

        if (!isJSONValid(mCommandJson)) {
            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_command_json_invalid));
            return false;
        }

        return true;
    }

    /**
     * Checks if the JSON input has the correct format
     *
     * @param input the JSON input which was entered
     * @return true if JSON format is correct, false otherwise
     */
    private boolean isJSONValid(String input) {
        if (TextUtils.isEmpty(input)) return false;
        try {
            new JSONObject(input);
        } catch (JSONException ex) {
            try {
                new JSONArray(input);
            } catch (JSONException ex1) {
                return false;
            }
        }
        return true;
    }
}
