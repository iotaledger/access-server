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

package com.de.xain.emdac.utils;

import com.de.xain.emdac.R;
import com.de.xain.emdac.main.model.VehicleInfo;
import com.google.gson.Gson;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.ArrayList;
import java.util.List;

@SuppressWarnings({"SpellCheckingInspection", "unused"})
public class VehicleInfoUtils {

    private static final String VD_DOOR_DRVR_RE_STS = "DoorDrvrReSts";
    private static final String VD_DOOR_DRVR_STS = "DoorDrvrSts";
    private static final String VD_VEH_MTN_ST_SAFE = "VehMtnStSafe";
    private static final String VD_ENG_COOLT_LVL = "EngCooltLvl";
    private static final String VD_TR_STS = "TrSts";
    private static final String VD_DOOR_PASS_STS = "DoorPassSts";
    private static final String VD_DOOR_PASS_RE_STS = "DoorPassReSts";
    private static final String VD_LOCKG_CEN_STS_FOR_USB_FB = "LockgCenStsForUsrFb";
    private static final String VD_VEH_SPD_LGT_SAFE = "VehSpdLgtSafe";
    private static final String VD_FU_LVL_INDCD_VAL = "FuLvlIndcdVal";
    private static final String VD_ENG_N = "EngN";
    private static final String VD_BKP_OF_DST_TRVLD = "BkpOfDstTrvld";
    private static final String VD_AMB_T_INDCD = "AmbTIndcd";
    private static final String VD_VEH_SPD_INDCD_VAL = "VehSpdIndcdVal";
    private static final String VD_VIN_BLK = "VinBlk";
    private static final String VD_VIN_POSN_1 = "VinPosn1";
    private static final String VD_VIN_POSN_2 = "VinPosn2";
    private static final String VD_VIN_POSN_3 = "VinPosn3";
    private static final String VD_VIN_POSN_4 = "VinPosn4";
    private static final String VD_VIN_POSN_5 = "VinPosn5";
    private static final String VD_VIN_POSN_6 = "VinPosn6";
    private static final String VD_VIN_POSN_7 = "VinPosn7";
    private static final String VD_TR_LOCK_STS = "TrLockSts";
    private static final String VD_VEH_BATT_U_SYS_U = "VehBattUSysU";
    private static final String VD_AMB_T_VAL_AT_DRV_SIDE = "AmbTValAtDrvrSide";
    private static final String VD_AMB_T_U_RAW_AT_LE_SIDE_FOR_ODB = "AmbTURawAtLeSideForObd";
    private static final String VD_AMB_T_U_RAW_AT_RI_SIDE_FOR_ODB = "AmbTURawAtRiSideForObd";
    private static final String VD_AMB_T_AMB_T_VAL_AT_PASS_SIDE = "AmbTValAtPassSide";
    private static final String VD_DRVR_BRK_TQ_AT_WHLS_REQD = "DrvrBrkTqAtWhlsReqd";
    private static final String VD_BRK_PEDL_PSD = "BrkPedlPsd";
    private static final String VD_BRK_TQ_TOT_REQ_FOR_PT = "BrkTqTotReqForPt";
    private static final String VD_GEAR_INDCN = "GearIndcn";
    private static final String VD_DRVR_PRPSN_TQ_REQ = "DrvrPrpsnTqReq";
    private static final String VD_CLU_PEDL_RAT = "CluPedlRat";

    public static List<VehicleInfo> getDefaultList(ResourceProvider resourceProvider) {
        List<VehicleInfo> vehicleInfoList = new ArrayList<>();

        vehicleInfoList.add(new VehicleInfo(VD_AMB_T_INDCD, resourceProvider.getString(R.string.vd_ambient_air_temperature)));
        vehicleInfoList.add(new VehicleInfo(VD_FU_LVL_INDCD_VAL, resourceProvider.getString(R.string.vd_fuel_tank_level)));
        vehicleInfoList.add(new VehicleInfo(VD_LOCKG_CEN_STS_FOR_USB_FB, resourceProvider.getString(R.string.vd_central_locking_status_for_user_feedback)));
        vehicleInfoList.add(new VehicleInfo(VD_TR_STS, resourceProvider.getString(R.string.vd_trunk_status)));
        vehicleInfoList.add(new VehicleInfo(VD_DOOR_DRVR_STS, resourceProvider.getString(R.string.vd_driver_door_status)));
        vehicleInfoList.add(new VehicleInfo(VD_DOOR_DRVR_RE_STS, resourceProvider.getString(R.string.vd_driver_door_rear_status)));
        vehicleInfoList.add(new VehicleInfo(VD_DOOR_PASS_STS, resourceProvider.getString(R.string.vd_passenger_door_status)));
        vehicleInfoList.add(new VehicleInfo(VD_DOOR_PASS_RE_STS, resourceProvider.getString(R.string.vd_passenger_door_rear_status)));
        vehicleInfoList.add(new VehicleInfo(VD_DRVR_BRK_TQ_AT_WHLS_REQD, resourceProvider.getString(R.string.vd_requested_brake_torque_at_wheels)));
        vehicleInfoList.add(new VehicleInfo(VD_DRVR_PRPSN_TQ_REQ, resourceProvider.getString(R.string.vd_requested_propulsion_torque)));
        vehicleInfoList.add(new VehicleInfo(VD_CLU_PEDL_RAT, resourceProvider.getString(R.string.vd_clutch_pedal_position)));
        vehicleInfoList.add(new VehicleInfo(VD_BRK_PEDL_PSD, resourceProvider.getString(R.string.vd_brake_pedal_pressed)));

        return vehicleInfoList;
    }

    public static List<VehicleInfo> combineSelected(JSONArray jsonArray, ResourceProvider resourceProvider) {
        List<VehicleInfo> vehicleInfoList = VehicleInfoUtils.getDefaultList(resourceProvider);
        Gson gson = new Gson();
        for (int i = 0, jsonArrayLength = jsonArray.length(); i < jsonArrayLength; i++) {
            try {
                String name = jsonArray.getString(i);
                for (int j = 0, vehicleInfoListSize = vehicleInfoList.size(); j < vehicleInfoListSize; j++) {
                    if (vehicleInfoList.get(j).getId().equalsIgnoreCase(name)) {
                        vehicleInfoList.get(j).setSelected(true);
                    }
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
        return vehicleInfoList;
    }
}
