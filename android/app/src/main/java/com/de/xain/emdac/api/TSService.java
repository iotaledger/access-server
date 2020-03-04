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

import com.de.xain.emdac.api.model.token_server.TSEmptyResponse;
import com.de.xain.emdac.api.model.token_server.TSFundRequest;
import com.de.xain.emdac.api.model.token_server.TSFundResponse;
import com.de.xain.emdac.api.model.token_server.TSSendResponse;
import com.de.xain.emdac.api.model.token_server.TSSendRequest;
import com.de.xain.emdac.api.model.token_server.TSBalanceRequest;
import com.de.xain.emdac.api.model.token_server.TSBalanceResponse;
import com.de.xain.emdac.api.model.token_server.TSDataResponse;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.PUT;

/**
 * Interface for communication with Token Server
 */
public interface TSService {
    @PUT("/balance")
    Call<TSDataResponse<TSBalanceResponse>> getBalance(@Body TSBalanceRequest request);

    @PUT("/fund")
    Call<TSEmptyResponse> fundAccount(@Body TSFundRequest request);

    @PUT("/send")
    Call<TSEmptyResponse> sendTokens(@Body TSSendRequest request);
}
