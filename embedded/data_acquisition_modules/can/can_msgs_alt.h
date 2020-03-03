/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _CAN_MSGS_H_
#define _CAN_MSGS_H_

// can_id = 0x40
struct CEMBodyFr02 {
    unsigned VehMtnStSafe:3;
    unsigned VehMtnStCntr:4;
    unsigned VehMtnStSafeGroup_UB:1;
    unsigned VehMtnStChks:8;
    unsigned DoorDrvrReSts:2;
    unsigned DoorDrvrReSts_UB:1;
    unsigned DoorDrvrSts_UB:1;
    unsigned DoorDrvrSts:2;
} __attribute__((packed));

// can_id = 0xB0
// belt lock status
struct CEMBodyFr05 {
    unsigned __padding0:8;
    unsigned ChrgnTyp:8;
    unsigned __padding1:8;
    unsigned BltLockStAtRowThrdRiForBltLockSts:1;
    unsigned BltLockStAtRowThrdRiForBltLockSt1:1;
    unsigned BltLockStAtRowThrdRiForBltLockEquid:1;
    unsigned BltLockStAtRowThrdRi_UB:1;
    unsigned BrkFldLvl_UB:1;
    unsigned BrkFldLvl:1;
    unsigned EngCooltLvl_UB:1
    unsigned EngCooltLvl:1;
    unsigned BltLockStAtRowSecRiForBltLockSts:1;
    unsigned BltLockStAtRowSecRiForBltLockSt1:1;
    unsigned BltLockStAtRowSecRiForBltLockEquid:1;
    unsigned BltLockStAtRowSecRi_UB:1;
    unsigned BltLockStAtRowSecMidForBltLockSts:1;
    unsigned BltLockStAtRowSecMidForBltLockSt1:1;
    unsigned BltLockStAtRowSecMidForBltLockEquid:1;
    unsigned BltLockStAtRowSecMid_UB:1;
    unsigned BltLockStAtPassForBltLockSts:1;
    unsigned BltLockStAtPassForBltLockSt1:1;
    unsigned BltLockStAtPass_UB:1;
    unsigned BltLockStAtRowThrdLe_UB:1;
    unsigned BltLockStAtRowSecLeForBltLockSts:1;
    unsigned BltLockStAtRowSecLeForBltLockSt1:1;
    unsigned BltLockStAtRowSecLeForBltLockEquid:1;
    unsigned BltLockStAtRowSecLe_UB:1;
    unsigned BltLockStAtRowThrdLeForBltLockSts:1;
    unsigned BltLockStAtRowThrdLeForBltLockSt1:1;
    unsigned BltLockStAtRowThrdLeForBltLockEquid:1;
    unsigned ChrgnTyp_UB:1;
    unsigned __padding2:4;
    unsigned BltLockStAtDrvrForDevErrSts2:1;
    unsigned BltLockStAtDrvrForBltLockSt1:1;
    unsigned BltLockStAtDrvr_UB:1;
} __attribute__((packed));

// can_id = 0xE0
struct CEMBodyFr11 {
    unsigned __padding0:12;
    unsigned DoorPassReSts:2;
    unsigned DoorPassReSts_UB:1;
    unsigned __padding1:14;
    unsigned DoorPassSts_UB:1;
    unsigned DoorPassSts:2;
    unsigned __padding2:13;
    unsigned TrSts_UB:1;
    unsigned TrSts:2;
} __attribute__((packed));

// can_id = 0x100
struct CEMBodyFr13 {
    unsigned __padding0:8;
    unsigned LockgCenSts_UB:1;
    unsigned LockgCenStsUpdEve:1;
    unsigned LockgCenStsLockSt:2;
    unsigned __padding1:14;
    unsigned DefrstSts_UB:1;
    unsigned __padding2:1;
    unsigned LockgCenStsForUsrFb:3;
    unsigned LockgCenStsForUsrFb_UB:1;
    unsigned __padding3:12;
    unsigned HoodSts:2;
    unsigned HoodSts_UB:1;
} __attribute__((packed));

// can_id = 0x1B0
struct CEMBodyFr17 {
    unsigned VehSpdLgtSafe_msb:7;
    unsigned VehSpdLgtSafeGroup_UB:1;
    unsigned VehSpdLgtSafe_lsb:8;
    unsigned VehSpdLgtSafeChks:8;
    unsigned __padding0:2;
    unsigned VehSpdLgtSafeQf:2;
    unsigned VehSpdLgtSafeCntr:4;
} __attribute__((packed));

// can_id = 0x250
struct CEMBodyFr23 {
    unsigned __padding0:16;
    unsigned DstToSrv_msb:7;
    unsigned __padding1:1;
    unsigned DstToSrv_lsb:8;
    unsigned __padding2:6;
    unsigned VehSpdIndcdQly:2;
    unsigned __padding3:15;
    unsigned VehSpdIndcdQly_UB:1;
} __attribute__((packed));

// can_id = 0x270
struct CEMBodyFr25 {
    unsigned __padding0:23;
    unsigned FuLvlIndcd_UB:1;
    unsigned __padding1:16;
    unsigned FuLvlIndcdVal_msb:2;
    unsigned FuLvlIndcdQly:2;
    unsigned EngSt1WdSts:4;
    unsigned FuLvlIndcdVal_lsb:8;
    unsigned TankFlapSts:2;
    unsigned TankFlapSts_UB:1;
    unsigned __padding2:4;
    unsigned EngSt1WdSts_UB:1;
} __attribute__((packed));

// can_id = 0x300
struct CEMBodyFr26 {
    unsigned EngNSafeEngNGrdt:16;
    unsigned EngN_msb:7;
    unsigned __padding0:1;
    unsigned EngN_lsb:8;
    unsigned EngNChks:8;
    unsigned __padding1:4;
    unsigned EngNCntr:4
    unsigned __padding2:8;
    unsigned DrvModReq:4;
    unsigned DrvModReq_UB:1;
    unsigned __padding3:1;
    unsigned __EngNSafe_UB:1;
} __attribute__((packed));

// can_id = 0x310
struct CEMBodyFr27 {
    unsigned BkpOfDstTrvld_msb:5;
    unsigned BkpOfDstTrvld_UB:1;
    unsigned __padding0:2;
    unsigned BkpOfDstTrvld_lsb:16;
    unsigned __padding1:18;
    unsigned DstSho:2;
    unsigned DstLong:2;
    unsigned __padding2:2;
    unsigned SpdUnit:2;
    unsigned __padding3:3;
    unsigned __FuCnsUnit:3
} __attribute__((packed));

// can_id = 0x1D0
struct CEMBodyFr28 {
    unsigned AmbTIndcd_msb:4;
    unsigned AmbTIndcdQf:2;
    unsigned AmbTIndcdUnit:2;
    unsigned AmbTIndcd_lsb:8;
    unsigned VehSpdIndcdVal_msb:1;
    unsigned VehSpdIndcdUnit:2;
    unsigned VehSpdIndcdUB:1;
    unsigned AmbTRaw_UB:1;
    unsigned __padding0:2;
    unsigned AmbTIndcdWithUnit_UB:1;
    unsigned VehSpdIndcdVal_lsb:8;
    unsigned __padding1:16;
    unsigned AmbTRawVal_msb:3;
    unsigned AmbTRawQly:2;
    unsigned __padding2:3;
    unsigned AmbTRawVal_lsb:8;
} __attribute__((packed));

// can_id = 0x360
struct CEMBodyFr31 {
    unsigned VinBlk:8;
    unsigned VinPosn1:8;
    unsigned VinPosn2:8;
    unsigned VinPosn3:8;
    unsigned VinPosn4:8;
    unsigned VinPosn5:8;
    unsigned VinPosn6:8;
    unsigned VinPosn7:8;
} __attribute__((packed));

// can_id = 0x380
struct CEMBodyFr32 {
    unsigned __padding0:38;
    unsigned CurtReDirCmd_UB:1;
    unsigned __padding1:11;
    unsigned CurtReDirCmd:2;
    unsigned __padding2:7;
    unsigned TrLockSts_UB:1;
    unsigned TrLockSts:2;
} __attribute__((packed));

// can_id = 0x330
struct CEMBodyFr33 {
    unsigned __padding0:4;
    unsigned TirePReLe:2;
    unsigned __padding1:1;
    unsigned ProfPenSts1_UB:1;
    unsigned TirePReRi:2;
    unsigned __padding2:2;
    unsigned TirePFrntRi:2;
    unsigned TirePFrntLe:2;
    unsigned __padding3:4;
    unsigned ProfPenSts1:4;
    unsigned __padding4:8;
    unsigned VehBattUSysU:8;
    unsigned __padding5:5;
    unsigned VehBattUGroup_UB:1;
    unsigned VehBattUSysUQf:2;
} __attribute__((packed));

// can_id = 0x259
struct CEMBodyFr44 {
    unsigned AmbTOutOfVehAmbTOutOfVeh_msb:3;
    unsigned AmbTOutOfVehAmbTOutOfVehQf:2;
    unsigned AmbTOutOfVeh_UB:1;
    unsigned __padding0:2;
    unsigned AmbTOutOfVehAmbTOutOfVeh_lsb:8;
} __attribute__((packed));

// can_id = 0x95
struct POTBodyFr02 {
    unsigned TrOpenerSts:4;
    unsigned TrOpenerSts_UB:1;
} __attribute__((packed));

// can_id = 0x1E0
struct DDMBodyFr02 {
    unsigned AmbTValAtDrvrSide_msb:8;
    unsigned __padding0:2;
    unsigned AmbTRawAtDrvrSide_UB:1;
    unsigned AmbTQlyAtDrvrSide:2;
    unsigned AmbTValAtDrvrSide_lsb:3;
    unsigned __padding1:8;
    unsigned WinPosnStsDrv:5;
    unsigned WinPosnStsDrv_UB:1;
    unsigned __padding2:10;
    unsigned DrvWinPosnStsAvailability:3;
    unsigned DrvWinPosnSts_UB:1;
    unsigned __padding3:5;
    unsigned DrvWinPosnStsValue:7;
} __attribute__((packed));

// can_id = 0x130
struct DDMBodyFr03 {
    unsigned __padding0:48;
    unsigned AmbTURawAtLeSideForObd_msb:2;
    unsigned AmbTURawAtLeSideForObd_UB:1;
    unsigned __padding1:5;
    unsigned AmbTURawAtLeSideForObd_lsb:8;
} __attribute__((packed));

// can_id = 0x160
struct PDMBodyFr02 {
    unsigned AmbTValAtPassSide_msb:3;
    unsigned AmbTQlyAtPassSide:2;
    unsigned AmbTRawAtPassSide_UB:1;
    unsigned __padding0:2;
    unsigned AmbTValAtPassSide_lsb:8;
    unsigned __padding1:3;
    unsigned WinPosnStsPass:5;
    unsigned __padding2:5;
    unsigned WinPosnStsPass_UB:1;
    unsigned __padding3:10;
    unsigned AmbTURawAtRiSideForObd_msb:2;
    unsigned __padding4:4;
    unsigned AmbTURawAtRiSideForObd_UB:1;
    unsigned __padding5:1;
    unsigned AmbTURawAtRiSideForObd_lsb:8;
} __attribute__((packed));

// can_id = 0xD0
struct VDDMChasFr04 {
    unsigned __padding0:16;
    unsigned PrpsnTqForBrkRels_msb:7;
    unsigned __padding1:1;
    unsigned PrpsnTqForBrkRels_lsb:8;
    unsigned __padding2:17;
    unsigned PrpsnTqForBrkRels_UB:1;
} __attribute__((packed));

// can_id = 0xE0
struct VDDMChasFr05 {
    unsigned DrvrBrkTqAtWhlsReqd:16;
    unsigned DrvrBrkTqAtWhlsReqdDrvrBrkTqAtWhlsReqdChks:8;
    unsigned __padding0:1;
    unsigned DrvrBrkTqAtWhlsReqdGroup_UB:1;
    unsigned DrvrBrkTqAtWhlsReqdQf:2;
    unsigned DrvrBrkTqAtWhlsReqdDrvrBrkTqAtWhlsReqdCntr:4;
    unsigned VehSpdLgtSafe_msb:7;
    unsigned VehSpdLgtSafeGroup_UB:1;
    unsigned VehSpdLgtSafe_lsb:8;
    unsigned VehSpdLgtSafeChks:8;
    unsigned __padding1:2;
    unsigned VehSpdLgtSafeQf:2;
    unsigned VehSpdLgtSafeCntr:4;
} __attribute__((packed));

// can_id = 0xF0
struct VDDMChasFr06 {
    unsigned __padding0:2;
    unsigned HoodSts_UB:1;
    unsigned DoorPassSts_UB:1;
    unsigned DoorPassReSts_UB:1;
    unsigned DoorDrvrSts_UB:1;
    unsigned __padding1:42;
    unsigned DoorPassSts:2;
    unsigned DoorPassReSts:2;
    unsigned DoorDrvrSts:2;
    unsigned __padding2:2;
    unsigned TrSts_UB:1;
    unsigned __padding3:3;
    unsigned TrSts:2;
    unsigned HoodSts:2;
} __attribute__((packed));

// can_id = 0x3A0
struct VDDMChasFr08 {
    unsigned __padding0:9;
    unsigned AmbTIndcdWithUnit_UB:1;
    unsigned __padding1:6;
    unsigned AmbTIndcd_msb:4;
    unsigned AmbTIndcdQf:2;
    unsigned AmbTIndcdUnit:2;
    unsigned AmbTIndcd_lsb:8;
    unsigned __padding2:21;
    unsigned DoorDrvrReSts_UB:1;
    unsigned DoorDrvrReSts:2;
} __attribute__((packed));

// can_id = 0x1C0
struct VDDMChasFr12 {
    unsigned BrkPedlPsdChks:8;
    unsigned BrkPedlNotPsdSafe:1;
    unsigned BrkPedlPsd:1;
    unsigned BrkPedlPsdQf:2;
    unsigned BrkPedlPsdCntr:4;
    unsigned __padding0:7;
    unsigned BrkPedlPsdSafeGroup_UB:1;
} __attribute__((packed));

// can_id = 0x1F0
struct VDDMChasFr16 {
    unsigned BkpOfDstTrvld_msb:16;
    unsigned __padding0:3;
    unsigned BkpOfDstTrvld_lsb:5;
    unsigned __padding1:12;
    unsigned DrvModReq:4;
    unsigned __padding2:11;
    unsigned DrvModReq_UB:1;
    unsigned __padding3:1;
    unsigned BkpOfDstTrvld_UB:1;
} __attribute__((packed));

// can_id = 0x240
struct VDDMChasFr17 {
    unsigned FuLvlIndcdVal_msb:2;
    unsigned FuLvlIndcdQly:2;
    unsigned __padding0:4;
    unsigned FuLvlIndcdVal_lsb:8;
    unsigned __padding1:23;
    unsigned FuLvlIndcd_UB:1;
} __attribute__((packed));

// can_id = 0x2E0
struct VDDMChasFr19 {
    unsigned VehModMngtGlbSafe1Chks:8;
    unsigned UsgModSts:4;
    unsigned VehModMngtGlbSafe1Cntr:4;
    unsigned PwrLvlElecMai:4;
    unsigned PwrLvlElecSubtyp:4;
    unsigned EgyLvlElecMai:4;
    unsigned EgyLvlElecSubtyp:4;
    unsigned VehModMngtGlbSafe1_UB:1;
    unsigned __padding0:1;
    unsigned CarModSubtypWdCarModSubtyp:3;
    unsigned CarModSts1:3;
    unsigned VehMtnStSafeGroup_UB:1;
    unsigned VehMtnStSafe:3;
    unsigned VehMtnStCntr:4;
} __attribute__((packed));

// can_id = 0x380
struct VDDMChasFr25 {
    unsigned __padding0:48;
    unsigned VehBattUSysUQf:2;
    unsigned EngCooltLvl_UB:1;
    unsigned EngCooltLvl:1;
    unsigned __padding1:2;
    unsigned VehBattUGroup_UB:1;
    unsigned __padding2:1;
    unsigned VehBattUSysU:8;
} __attribute__((packed));

// can_id = 0x1A
struct VDDMChasFr48 {
    unsigned __padding0:32;
    unsigned BrkTqCntr:4;
    unsigned BrkTq_UB:1;
    unsigned __padding1:3;
    unsigned BrkTqTotReqForPt_msb:5;
    unsigned __padding2:3;
    unsigned BrkTqTotReqForPt_lsb:8;
    unsigned BrkTqChks:8;
} __attribute__((packed));

// can_id = 0x15
struct VDDMChasFr49 {
    unsigned BrkSysCylPMstAct_msb:2;
    unsigned BrkSysCylPMst_UB:1;
    unsigned WhlBrkPEstimdMax_UB:1;
    unsigned __padding0:4;
    unsigned BrkSysCylPMstAct_lsb:8;
    unsigned BrkSysCylPMstTar_msb:2;
    unsigned BrkSysCylPMstCntr:4;
    unsigned BrkSysCylPMstActQf:2;
    unsigned BrkSysCylPMstTar_lsb:8;
    unsigned BrkSysCylPMstVirt_msb:2;
    unsigned BrkSysCylPMstVirtQf:2;
    unsigned __padding1:4;
    unsigned BrkSysCylPMstVirt_lsb:8;
    unsigned BrkSysCylPMstChks:8;
    unsigned WhlBrkPEstimdMax:8;
} __attribute__((packed));

// can_id = 0x35
struct ECMChasFr03 {
    unsigned PrpsnTqReReq:16;
    unsigned AccrPedlRatGrdt_msb:8;
    unsigned AccrPedlRatGrdt_UB:1;
    unsigned AccrPedlRatGrdt_lsb:7;
    unsigned __padding0:23;
    unsigned PrpsnTqReReq_UB:1;
} __attribute__((packed));

// can_id = 0x100
struct ECMChasFr04 {
    unsigned GearTarIndcn:4;
    unsigned GeearIndcn:4;
    unsigned __padding0:5;
    unsigned AccrPedlPsd_UB:1;
    unsigned GearShiftIndcn:2;
    unsigned __padding1:32;
    unsigned AccrPedlPsdAccrPedlPsdChks:8;
    unsigned __padding2:1;
    unsigned GearIndcnRec_UB:1;
    unsigned AccrPedlPsdAccrPedlPsdSts:1;
    unsigned AccrPedlPsdAccrPedlPsd:1;
    unsigned AccrPedlPsdAccrPedlPsdCntr:4;
} __attribute__((packed));

// can_id = 0x110
struct ECMChasFr05 {
    unsigned EngNGrdt1:16;
    unsigned EngNGrdt1Chks:8;
    unsigned __padding0:4;
    unsigned EngNGrdt1Cntr:4;
    unsigned __padding1:10;
    unsigned EngNGrdt1Safe_UB:1;
} __attribute__((packed));

// can_id = 0x130
struct ECMChasFr06 {
    unsigned EngNSafeEngNGrdt:16;
    unsigned EngN_msb:7;
    unsigned __padding0:1;
    unsigned EngN_lsb:8;
    unsigned EngNChks:8;
    unsigned __padding1:2;
    unsigned EngNSafe_UB:1;
    unsigned DrvrPrpsnTqReq_UB:1;
    unsigned EngNCntr:4;
    unsigned DrvrPrpsnTqReq:16;
} __attribute__((packed));

// can_id = 0x210
struct ECMChasFr10 {
    unsigned __padding0:16;
    unsigned CluPedlRat:8;
    unsigned __padding1:14;
    unsigned CluPedlRat_UB:1;
} __attribute__((packed));

// can_id = 0x23A
struct ECMChasFr22 {
    unsigned __padding0:24;
    unsigned PrpsnTqDirCntr:4;
    unsigned PrpsnTqDirAct:2;
    unsigned PrpsnTqDir_UB:1;
    unsigned __padding1:1;
    unsigned PrpsnTqDirChks:8;
} __attribute__((packed));

#endif
