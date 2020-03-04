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

package com.de.xain.emdac.main.ui;

import android.content.Intent;
import android.speech.RecognizerIntent;
import android.support.annotation.Nullable;
import android.util.Pair;

import com.de.xain.emdac.CommunicationViewModel;
import com.de.xain.emdac.R;
import com.de.xain.emdac.api.Communicator;
import com.de.xain.emdac.api.PSService;
import com.de.xain.emdac.api.TSService;
import com.de.xain.emdac.api.asr.ASRClient;
import com.de.xain.emdac.api.model.Command;
import com.de.xain.emdac.api.model.CommandAction;
import com.de.xain.emdac.api.model.CommunicationMessage;
import com.de.xain.emdac.api.model.TCPResponse;
import com.de.xain.emdac.api.model.UDPResponse;
import com.de.xain.emdac.api.model.policy_server.PSClearPolicyListRequest;
import com.de.xain.emdac.api.model.policy_server.PSEmptyResponse;
import com.de.xain.emdac.api.model.token_server.TSEmptyResponse;
import com.de.xain.emdac.api.model.token_server.TSFundRequest;
import com.de.xain.emdac.api.model.token_server.TSSendRequest;
import com.de.xain.emdac.api.tcp.TCPClient;
import com.de.xain.emdac.api.udp.UDPClient;
import com.de.xain.emdac.user.UserManager;
import com.de.xain.emdac.utils.Constants;
import com.de.xain.emdac.utils.JsonUtils;
import com.de.xain.emdac.utils.Optional;
import com.de.xain.emdac.utils.ResourceProvider;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonSyntaxException;
import com.google.gson.reflect.TypeToken;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.subjects.BehaviorSubject;
import io.reactivex.subjects.PublishSubject;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import timber.log.Timber;

@SuppressWarnings({"WeakerAccess", "FieldCanBeLocal"})
public class CommandListViewModel extends CommunicationViewModel {

    public static final int REFILL_AMOUNT = 1;

    private final String COMMAND_CLEAR_POLICY = "clear_policy_list";
    private final String DOOR_COMPONENT = "door";
    private final String TRUNK_COMPONENT = "trunk";
    private final String JSON_RESPONSE_TAG = "response";
    private final String ACCESS_GRANTED = "access granted";
    private final UDPClient mUDPClient;
    private final ASRClient mASRClient;
    private final UserManager mUserManager;
    private final Gson mGson;
    private final TSService mTSService;
    private final PSService mPSService;

    private final BehaviorSubject<Optional<List<Command>>> mCommandList = BehaviorSubject.createDefault(new Optional<>(null));
    private final PublishSubject<Boolean> mShowRefresh = PublishSubject.create();

    private Call<TSEmptyResponse> mAccountRefillCall;
    private Call<TSEmptyResponse> mSendTokenResponseBodyCall;

    @Nullable
    private Command mInvokedCommand;

    @Nullable
    private String policyIdToEnable;

    private boolean mPolicyRequested = false;

    @Inject
    public CommandListViewModel(Communicator communicator,
                                UDPClient udpClient,
                                ResourceProvider resourceProvider,
                                ASRClient asrClient,
                                UserManager userManager,
                                TSService tsService,
                                PSService psService,
                                Gson gson) {
        super(communicator, resourceProvider);
        mPSService = psService;
        mUDPClient = udpClient;
        mASRClient = asrClient;
        mUserManager = userManager;
        mTSService = tsService;
        mGson = gson;
    }

    public Optional<List<Command>> getListOfCommands() { return mCommandList.getValue(); }

    public Observable<Optional<List<Command>>> getObservableCommandList() {
        return mCommandList;
    }

    public Optional<List<Command>> getCommandList() {
        return mCommandList.getValue();
    }

    public Observable<Boolean> getShowRefresh() {
        return mShowRefresh;
    }

    public boolean isPolicyRequested() {
        return mPolicyRequested;
    }

    /**
     * Asynchronously sends command to board to executeCommand it.
     *
     * @param command command which should be executed
     */
    public void executeCommand(Command command) {
        mInvokedCommand = command;
        String policy = command.getActiveAction().getPolicyId();
        sendTCPMessage(CommunicationMessage.makeResolvePolicyRequest(policy), mResourceProvider.getString(R.string.msg_executing_command, command.getActionName()));
    }

    public void getPolicyList() {
        if (mUserManager.getUser() == null)
            return;
        mPolicyRequested = true;
        mShowRefresh.onNext(true);
        String userId = mUserManager.getUser().getPublicId();
        sendTCPMessage(CommunicationMessage.makePolicyListRequest(userId));
    }

    public void enablePolicy(String policyId) {
        policyIdToEnable = policyId;
        sendTCPMessage(CommunicationMessage.makeEnablePolicyRequest(policyId));
    }

    public void payPolicy(TSSendRequest requestBody, String policyId) {
        mSendTokenResponseBodyCall = mTSService.sendTokens(requestBody);
        mSendTokenResponseBodyCall.enqueue(new Callback<TSEmptyResponse>() {
            @Override
            public void onResponse(Call<TSEmptyResponse> call, Response<TSEmptyResponse> response) {
                mShowLoading.onNext(new Pair<>(false, null));
                if (response.body() == null) {
                    mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_unable_to_pay));
                } else if (response.body().isError()) {
                    mShowDialogMessage.onNext(response.body().getMessage());
                } else {
                    enablePolicy(policyId);
                }
            }

            @Override
            public void onFailure(Call<TSEmptyResponse> call, Throwable t) {
                mShowLoading.onNext(new Pair<>(false, null));
                mShowDialogMessage.onNext(t.getLocalizedMessage());
            }
        });

        mShowLoading.onNext(new Pair<>(true, mResourceProvider.getString(R.string.msg_paying)));
    }

    public void refillAccount(String walletId) {
        mAccountRefillCall = mTSService.fundAccount(new TSFundRequest(walletId, REFILL_AMOUNT));
        mAccountRefillCall.enqueue(new Callback<TSEmptyResponse>() {
            @Override
            public void onResponse(Call<TSEmptyResponse> call, Response<TSEmptyResponse> response) {
                mShowLoading.onNext(new Pair<>(false, null));
                if (response.body() == null) {
                    mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_refilling_failed));
                } else if (response.body().isError()) {
                    mShowDialogMessage.onNext(response.body().getMessage());
                } else {
                    mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_refilling_successful));
                }
            }

            @Override
            public void onFailure(Call<TSEmptyResponse> call, Throwable t) {
                mShowLoading.onNext(new Pair<>(false, null));
                mShowDialogMessage.onNext(t.getLocalizedMessage());
            }
        });

        mShowLoading.onNext(new Pair<>(true, mResourceProvider.getString(R.string.msg_refilling_account)));
    }

    public void clearUserList() {
        sendTCPMessage(CommunicationMessage.makeClearAllUsersRequest(), mResourceProvider.getString(R.string.msg_clearing_users));
    }

    /**
     * Send clear policy list command to server.
     */
    public void clearPolicyList() {
        mShowLoading.onNext(new Pair<>(true, mResourceProvider.getString(R.string.msg_policy_list_clearing)));
        mPSService.clearPolicyList(new PSClearPolicyListRequest(mUserManager.getUser().getDeviceId()))
                .enqueue(new Callback<PSEmptyResponse>() {
                    @Override
                    public void onResponse(Call<PSEmptyResponse> call, Response<PSEmptyResponse> response) {
                        // check if request was done
                        mShowLoading.onNext(new Pair<>(false, null));
                        if (response.isSuccessful()) {
                            mSnackbarMessage.onNext(mResourceProvider.getString(R.string.msg_policy_list_cleared_successfully));
                        } else {
                            String message = "Response code: " + response.code() + "\n" + "Reason: " + response.message();
                            Timber.e(message);
                            mSnackbarMessage.onNext(mResourceProvider.getString(R.string.msg_policy_list_cleared_error));
                        }
                    }

                    @Override
                    public void onFailure(Call<PSEmptyResponse> call, Throwable t) {
                        mSnackbarMessage.onNext(t.getLocalizedMessage());
                        mShowLoading.onNext(new Pair<>(false, null));
                    }
                });
    }

    public Intent getAsrIntent() {
        return mASRClient.generateIntent();
    }

    public void onAsrResult(Intent data) {
        ArrayList<String> text = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
        if ((text != null) && (text.size() > 0)) {
            Command matchingCommand = findCommandByCommandName(text.get(0));

            if (matchingCommand != null) {
                executeCommand(matchingCommand);
            } else {
                mSnackbarMessage.onNext(mResourceProvider.getString(R.string.unknown_command));
            }
        }
    }

    /**
     * Tries to find command by it's action name.
     *
     * @param inputAction Name of action which is being searched in list of available actions.
     * @return If found, returns command which contains passed action, otherwise return null.
     */
    @Nullable
    private Command findCommandByCommandName(final String inputAction) {
        if (mCommandList.getValue().isEmpty())
            return null;

        for (Command command : mCommandList.getValue().get()) {
            for (CommandAction action : command.getActionList()) {
                if (inputAction.equalsIgnoreCase(action.getActionName())) {
                    command.setActiveAction(action);
                    return command;
                }
            }
        }
        return null;
    }

    @Override
    protected void handleTCPError(TCPClient.TCPError error) {
        super.handleTCPError(error);
        mShowRefresh.onNext(false);
    }

    @Override
    protected void handleTCPResponse(String sentMessage, String response) {
        super.handleTCPResponse(sentMessage, response);

        mShowRefresh.onNext(false);

        String cmd = CommunicationMessage.getCmdFromMessage(sentMessage);

        JsonElement jsonElement = JsonUtils.extractJsonElement(response);
        if (jsonElement == null) return;

        if (cmd != null) {
            switch (cmd) {
                case CommunicationMessage.CLEAR_ALL_USERS:
                    try {
                        TCPResponse<Object> tcpResponse = mGson.fromJson(jsonElement,
                                new TypeToken<TCPResponse<Object>>() {
                                }.getType());
                        if (tcpResponse.isSuccessful()) {
                            mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_users_list_cleared_successfully));
                        } else {
                            String message = tcpResponse.getMessage();
                            if (message == null)
                                message = mResourceProvider.getString(R.string.something_wrong_happened);
                            mShowDialogMessage.onNext(message);
                        }
                    } catch (JsonSyntaxException ignored) {
                        mShowDialogMessage.onNext(mResourceProvider.getString(R.string.msg_unable_to_clear_users));
                    }
                    break;
                case CommunicationMessage.ENABLE_POLICY:
                    if (policyIdToEnable != null) {
                        enablePolicyAfterServerResponse(policyIdToEnable);
                        policyIdToEnable = null;
                    }
                    break;
                case CommunicationMessage.GET_POLICY_LIST:
                    try {
                        JsonArray jsonArray = jsonElement.getAsJsonArray();
                        List<CommandAction> actionList = CommandAction.getListOfActions(jsonArray, mResourceProvider);
                        List<Command> commandList = Command.makeListOfCommands(actionList, mResourceProvider);
                        mCommandList.onNext(new Optional<>(commandList));
                        return;
                    } catch (IllegalStateException ignored) {
                    }
                    break;
                case CommunicationMessage.RESOLVE:
                    if (mInvokedCommand != null) {
                        String resolvedResponse = resolveResponse(response);
                        if (resolvedResponse != null) {
                            handleNewStatus(resolvedResponse, mInvokedCommand);
                        }
                        mInvokedCommand = null;
                    }
                    break;
            }
        }
    }

    private void enablePolicyAfterServerResponse(String policyId) {
        if (mCommandList.getValue().isEmpty())
            return;
        List<Command> commandList = mCommandList.getValue().get();
        for (int i = 0, n = commandList.size(); i < n; i++) {
            for (int j = 0, m = commandList.get(i).getActionList().length; j < m; j++) {
                if (commandList.get(i).getActionList()[j].getPolicyId().equalsIgnoreCase(policyId)) {
                    commandList.get(i).getActionList()[j].setPaid(true);
                }
            }
        }
        mCommandList.onNext(new Optional<>(commandList));
    }

    private String resolveResponse(String message) {
        try {
            JSONObject jsonObject = new JSONObject(message);
            if (jsonObject.has(JSON_RESPONSE_TAG)) {
                return jsonObject.getString(JSON_RESPONSE_TAG);
            }
        } catch (JSONException ignored) {
        }
        return null;
    }

    /**
     * Parses UDP message and changes status of sensors.
     *
     * @param message message received through UDP.
     */
    private void handleNewStatus(String message) {
        if (mCommandList.getValue().isEmpty())
            return;
        try {
            UDPResponse udpResponse = mGson.fromJson(message, UDPResponse.class);
            List<Command> commandList = mCommandList.getValue().get();
            if ((commandList != null) && (commandList.size() > 0)) {
                for (int i = 0; i < commandList.size(); i++) {
                    String action = commandList.get(i).getActiveAction().getAction();
                    if ((Constants.ACTION_CLOSE_DOOR.equalsIgnoreCase(action) || Constants.ACTION_OPEN_DOOR.equalsIgnoreCase(action)) && DOOR_COMPONENT.equalsIgnoreCase(udpResponse.getComponent())) {
                        commandList.get(i).setState(udpResponse.getState());
                    } else if (Constants.ACTION_OPEN_TRUNK.equalsIgnoreCase(action) && TRUNK_COMPONENT.equalsIgnoreCase(udpResponse.getComponent())) {
                        commandList.get(i).setState(udpResponse.getState());
                    }
                }
                mCommandList.onNext(new Optional<>(commandList));
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Changes status of sensors based on board's command response.
     *
     * @param message response received from device.
     * @param command command that was sent to device.
     */
    private void handleNewStatus(String message, Command command) {
        if (!ACCESS_GRANTED.equalsIgnoreCase(message)) return;

        if (mCommandList.getValue().isEmpty())
            return;

        List<Command> commandList = mCommandList.getValue().get();

        if (commandList.size() > 0) {
            for (int i = 0; i < commandList.size(); i++) {
                if (command.equals(commandList.get(i))) {
                    commandList.get(i).didExecute(command.getActiveAction());
                }
            }
            mCommandList.onNext(new Optional<>(commandList));
        }
    }

    private void connectUDP() {
        Thread thread = new Thread(mUDPClient);
        thread.setPriority(Thread.MAX_PRIORITY - 2);
        thread.start();
    }

    /**
     * Closes UDP connection.
     */
    private void disconnectUDP() {
        mUDPClient.stopClient();
    }

}
