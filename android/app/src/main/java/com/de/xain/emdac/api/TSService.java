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
