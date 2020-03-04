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

package com.de.xain.emdac.main.model;

import android.support.annotation.Nullable;
import android.util.Pair;

import com.de.xain.emdac.CommunicationViewModel;
import com.de.xain.emdac.R;
import com.de.xain.emdac.api.Communicator;
import com.de.xain.emdac.api.model.CommunicationMessage;
import com.de.xain.emdac.api.tcp.TCPClient;
import com.de.xain.emdac.main.task.TCPCommunicateTask;
import com.de.xain.emdac.utils.JsonUtils;
import com.de.xain.emdac.utils.ResourceProvider;
import com.de.xain.emdac.utils.VehicleInfoUtils;
import com.google.gson.JsonElement;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

import javax.inject.Inject;

import io.reactivex.Observable;
import io.reactivex.subjects.BehaviorSubject;
import io.reactivex.subjects.PublishSubject;

public class VehicleInfoListViewModel extends CommunicationViewModel {

    private final BehaviorSubject<List<VehicleInfo>> mVehicleInfoList = BehaviorSubject.createDefault(new ArrayList<>());
    private final PublishSubject<Boolean> mShowRefresh = PublishSubject.create();


    @Inject
    public VehicleInfoListViewModel(Communicator communicator, ResourceProvider resourceProvider) {
        super(communicator, resourceProvider);
    }

    public Observable<List<VehicleInfo>> getObservableVehicleInfoList() {
        return mVehicleInfoList;
    }

    public Observable<Boolean> getShowRefresh() {
        return mShowRefresh;
    }


    public void saveVehicleInfoList(List<VehicleInfo> vehicleInfoList) {
        List<VehicleInfo> selectedVehicleInfoList = new ArrayList<>();
        for (VehicleInfo vehicleInfo : vehicleInfoList)
            if (vehicleInfo.isSelected())
                selectedVehicleInfoList.add(vehicleInfo);
        sendTCPMessage(CommunicationMessage.makeVehicleInfoSaveRequest(selectedVehicleInfoList));
    }

    public void requestVehicleInfoList() {
        sendTCPMessage(CommunicationMessage.makeGetDatasetRequest());
        mShowRefresh.onNext(true);
    }

    @Override
    protected void handleTCPResponse(String sentMessage, String response) {
        super.handleTCPResponse(sentMessage, response);

        mShowRefresh.onNext(false);

        if (response == null) return;
        JsonElement jsonElement = JsonUtils.extractJsonElement(response);
        if (jsonElement == null) return;


        if (sentMessage == null) return;
        String cmd = CommunicationMessage.getCmdFromMessage(sentMessage);
        if (cmd == null) return;
        switch (cmd) {
            case CommunicationMessage.GET_DATASET:
                mVehicleInfoList.onNext(createVehicleInfoListFromServerResponse(jsonElement.toString()));
                break;
            case CommunicationMessage.SET_DATASET:
                if (isRequestGranted(response)) {
                    mVehicleInfoList.onNext(mVehicleInfoList.getValue());
                    mSnackbarMessage.onNext(mResourceProvider.getString(R.string.msg_saving_success));
                } else {
                    mSnackbarMessage.onNext(mResourceProvider.getString(R.string.msg_saving_failed));
                }
                break;
        }
    }

    @Override
    protected void handleTCPError(TCPClient.TCPError error) {
        super.handleTCPError(error);
        mShowRefresh.onNext(false);
    }

    private List<VehicleInfo> createVehicleInfoListFromServerResponse(String response) {
        try {
            JSONArray jsonArray = new JSONArray(response);
            return VehicleInfoUtils.combineSelected(jsonArray, mResourceProvider);
        } catch (JSONException ignored) {
        }
        return new ArrayList<>();
    }

    private boolean isRequestGranted(String response) {
        JSONObject jsonObject;
        try {
            jsonObject = new JSONObject(response);
            if (jsonObject.has("response") && jsonObject.get("response") instanceof String) {
                if (jsonObject.getString("response").equalsIgnoreCase("access granted")) {
                    return true;
                }
            }
        } catch (JSONException ignored) {
        }
        return false;
    }
}
