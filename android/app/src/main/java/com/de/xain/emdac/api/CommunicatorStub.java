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

package com.de.xain.emdac.api;

import android.os.CountDownTimer;
import android.os.UserManager;
import android.util.Pair;

import com.de.xain.emdac.api.model.CommunicationMessage;
import com.de.xain.emdac.api.model.TCPResponse;
import com.de.xain.emdac.api.tcp.TCPClient;
import com.de.xain.emdac.delegation.UserPublic;
import com.de.xain.emdac.models.GetUserIdResponse;
import com.de.xain.emdac.models.User;
import com.de.xain.emdac.models.UserUtils;
import com.de.xain.emdac.utils.JsonUtils;
import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import javax.inject.Inject;
import javax.inject.Singleton;

import io.reactivex.Observable;
import io.reactivex.subjects.PublishSubject;

@Singleton
public class CommunicatorStub implements Communicator {

    private JsonArray mDataset = new JsonArray();
    private PublishSubject<Pair<String, String>> mResponse = PublishSubject.create();
    private PublishSubject<TCPClient.TCPError> mTCPError = PublishSubject.create();
    private Random rand = new Random();
    private Gson gson = new Gson();

    private PSService mPsService;
    private TSService mTsService;

    @Inject
    public CommunicatorStub(TSService tsService, PSService psService) {
        mTsService = tsService;
        mPsService = psService;
    }

    @Override
    public void sendTCPMessage(String message) {
        int millis = rand.nextInt(1000) + 500;
        new CountDownTimer(millis, millis) {
            @Override
            public void onTick(long millisUntilFinished) {
            }

            @Override
            public void onFinish() {
                stubTCPMessage(message);
            }
        }.start();
    }

    private void stubTCPMessage(String message) {
        String cmd = CommunicationMessage.getCmdFromMessage(message);
        if (cmd != null) {
            switch (cmd) {
                case CommunicationMessage.GET_POLICY_LIST:
//                    mPsService.
                    mResponse.onNext(new Pair<>(message, "[{\"policy_id\":\"531565B8F40ECA5AAC55FEE6664A233E2D0312856B777D875EB9ACC4F85D1E99\",\"action\":\"close_door\"},{\"policy_id\":\"45C88599131156AED16B48FB6A57B8211F24B92877076164AC5D143A81730645\",\"action\":\"open_door\"}, {\"policy_id\":\"531565B8F40ECA5AAC55FEE6664A233E2D0312856B777D875EB9ACC4F85D1F00\",\"action\":\"honk\"},{\"policy_id\":\"531565B8F40ECA5AAC55FEE6664A233E2D0312856B777D875EB9ACC4F85D1F01\",\"action\":\"alarm_on\"},{\"policy_id\":\"531565B8F40ECA5AAC55FEE6664A233E2D0312856B777D875EB9ACC4F85D1F02\",\"action\":\"alarm_off\"}]"));
                    break;
                case CommunicationMessage.RESOLVE:
                    mResponse.onNext(new Pair<>(message, "{\"response\":\"access granted\"}"));
                    break;
                case CommunicationMessage.SET_DATASET: {
                    JsonElement jsonElement = JsonUtils.extractJsonElement(message);
                    if (jsonElement == null) return;
                    try {
                        mDataset = jsonElement.getAsJsonObject().get("dataset_list").getAsJsonArray();
                        mResponse.onNext(new Pair<>(message, "{\"response\":\"access granted\"}"));
                    } catch (IllegalStateException e) {
                        mResponse.onNext(new Pair<>(message, "{\"response\":\"access denied\"}"));
                    } catch (UnsupportedOperationException e) {
                        mResponse.onNext(new Pair<>(message, "{\"response\":\"access denied\"}"));
                    }
                }
                break;
                case CommunicationMessage.GET_DATASET:
                    mResponse.onNext(new Pair<>(message, mDataset.toString()));
                    break;
                case CommunicationMessage.GET_AUTH_USER_ID: {
                    TCPResponse<GetUserIdResponse> response = new TCPResponse<>();
                    response.setError(0);
                    response.setMessage("success");
                    GetUserIdResponse r = new GetUserIdResponse();
                    r.setUserId("123");
                    response.setData(r);
                    mResponse.onNext(new Pair<>(message, gson.toJson(response)));
                }
                break;
                case CommunicationMessage.GET_USER: {
                    TCPResponse<User> response = new TCPResponse<>();
                    response.setError(0);
                    response.setMessage("success");
                    User user = UserUtils.getDefaultUser("jamie");

                    mResponse.onNext(new Pair<>(message, gson.toJson(response)));
                }
                break;
                case CommunicationMessage.GET_ALL_USERS: {
                    TCPResponse<List<UserPublic>> tcpResponse = new TCPResponse<>();
                    List<UserPublic> users = new ArrayList<>();
                    users.add(new UserPublic("jamie", "3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51"));
                    users.add(new UserPublic("charlie", "3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51"));
                    users.add(new UserPublic("alex", "3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51"));
                    users.add(new UserPublic("richard", "3c9d985c5d630e6e02f676997c5e9f03b45c6b7529b2491e8de03c18af3c9d87f0a65ecb5dd8f390dee13835354b222df414104684ce9f1079a059f052ca6e51"));
                    tcpResponse.setError(0);
                    tcpResponse.setMessage("success");
                    tcpResponse.setData(users);
                    mResponse.onNext(new Pair<>(message, gson.toJson(tcpResponse)));
                }
                break;
                default:
                    mResponse.onNext(new Pair<>(message, ""));
            }
        }
    }

    @Override
    public Observable<Pair<String, String>> getObservableTCPResponse() {
        return mResponse;
    }

    @Override
    public Observable<TCPClient.TCPError> getObservableTCPError() {
        return mTCPError;
    }

    @Override
    public void disconnectTCP() {

    }
}
