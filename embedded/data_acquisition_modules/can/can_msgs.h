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

const int supported_chas_msgs[17];
const int supported_body_msgs[17];

// can_id = 0x40
struct CEMBodyFr02 {
    unsigned __padding0:13;
    unsigned DoorDrvrSts_UB:1;
    unsigned DoorDrvrSts:2;
    unsigned __padding1:2;
    unsigned DoorDrvrReSts:2;
    unsigned DoorDrvrReSts_UB:1;
    unsigned __padding2:27;
    unsigned VehMtnStChks:8;
    unsigned VehMtnStSafe:3;
    unsigned VehMtnStCntr:4;
    unsigned VehMtnStSafeGroup_UB:1;
} __attribute__((packed));

// can_id = 0xB0
struct CEMBodyFr05 {
    unsigned BltLockStAtDrvrForDevErrSts2:1;
    unsigned BltLockStAtDrvrForBltLockSt1:1;
    unsigned BltLockStAtDrvr_UB:1;
    unsigned __padding0:5;
    unsigned BltLockStAtRowThrdLeForBltLockSts:1;
    unsigned BltLockStAtRowThrdLeForBltLockSt1:1;
    unsigned BltLockStAtRowThrdLeForBltLockEquid:1;
    unsigned ChrgnTyp_UB:1;
    unsigned __padding1:4;
    unsigned BltLockStAtPassForBltLockSts:1;
    unsigned BltLockStAtPassForBltLockSt1:1;
    unsigned BltLockStAtPass_UB:1;
    unsigned BltLockStAtRowThrdLe_UB:1;
    unsigned BltLockStAtRowSecLeForBltLockSts:1;
    unsigned BltLockStAtRowSecLeForBltLockSt1:1;
    unsigned BltLockStAtRowSecLeForBltLockEquid:1;
    unsigned BltLockStAtRowSecLe_UB:1;
    unsigned BltLockStAtRowSecRiForBltLockSts:1;
    unsigned BltLockStAtRowSecRiForBltLockSt1:1;
    unsigned BltLockStAtRowSecRiForBltLockEquid:1;
    unsigned BltLockStAtRowSecRi_UB:1;
    unsigned BltLockStAtRowSecMidForBltLockSts:1;
    unsigned BltLockStAtRowSecMidForBltLockSt1:1;
    unsigned BltLockStAtRowSecMidForBltLockEquid:1;
    unsigned BltLockStAtRowSecMid_UB:1;
    unsigned BltLockStAtRowThrdRiForBltLockSts:1;
    unsigned BltLockStAtRowThrdRiForBltLockSt1:1;
    unsigned BltLockStAtRowThrdRiForBltLockEquid:1;
    unsigned BltLockStAtRowThrdRi_UB:1;
    unsigned BrkFldLvl_UB:1;
    unsigned BrkFldLvl:1;
    unsigned EngCooltLvl_UB:1;
    unsigned EngCooltLvl:1;
    unsigned __padding2:8;
    unsigned ChrgnTyp:8;
    unsigned __padding3:8;
} __attribute__((packed));

// can_id = 0xE0
struct CEMBodyFr11 {
    unsigned __padding0:21;
    unsigned TrSts_UB:1;
    unsigned TrSts:2;
    unsigned __padding1:13;
    unsigned DoorPassSts_UB:1;
    unsigned DoorPassSts:2;
    unsigned __padding2:12;
    unsigned DoorPassReSts:2;
    unsigned DoorPassReSts_UB:1;
    unsigned __padding3:9;
} __attribute__((packed));

// can_id = 0x100
struct CEMBodyFr13 {
    unsigned __padding0:8;
    unsigned HoodSts:2;
    unsigned HoodSts_UB:1;
    unsigned __padding1:13;
    unsigned LockgCenStsForUsrFb:3;
    unsigned LockgCenStsForUsrFb_UB:1;
    unsigned __padding2:10;
    unsigned DefrstSts_UB:1;
    unsigned __padding3:9;
    unsigned LockgCenSts_UB:1;
    unsigned LockgCenStsUpdEve:1;
    unsigned LockgCenStsLockSt:2;
    unsigned __padding4:12;
} __attribute__((packed));

// can_id = 0x1B0
struct CEMBodyFr17 {
    unsigned long long __padding0:34;
    unsigned VehSpdLgtSafeQf:2;
    unsigned VehSpdLgtSafeCntr:4;
    unsigned VehSpdLgtSafeChks:8;
    unsigned VehSpdLgtSafe:15;
    unsigned VehSpdLgtSafeGroup_UB:1;
} __attribute__((packed));

// can_id = 0x250
struct CEMBodyFr23 {
    unsigned __padding0:15;
    unsigned VehSpdIndcdQly_UB:1;
    unsigned __padding1:14;
    unsigned VehSpdIndcdQly:2;
    unsigned DstToSrv:15;
    unsigned __padding2:17;
} __attribute__((packed));

// can_id = 0x270
struct CEMBodyFr25 {
    unsigned TankFlapSts:2;
    unsigned TankFlapSts_UB:1;
    unsigned __padding0:4;
    unsigned EngSt1WdSts_UB:1;
    unsigned FuLvlIndcdVal:10;
    unsigned FuLvlIndcdQly:2;
    unsigned EngSt1WdSts:4;
    unsigned __padding1:23;
    unsigned FuLvlIndcd_UB:1;
    unsigned __padding2:16;
} __attribute__((packed));

// can_id = 0x300
struct CEMBodyFr26 {
    unsigned DrvModReq:4;
    unsigned DrvModReq_UB:1;
    unsigned __padding0:1;
    unsigned EngNSafe_UB:1;
    unsigned __padding1:13;
    unsigned EngNCntr:4;
    unsigned EngNChks:8;
    unsigned EngN:15;
    unsigned __padding2:1;
    unsigned EngNSafeEngNGrdt:16;
} __attribute__((packed));

// can_id = 0x310
struct CEMBodyFr27 {
    unsigned __padding0:8;
    unsigned SpdUnit:2;
    unsigned __padding1:3;
    unsigned FuCnsUnit:3;
    unsigned __padding2:2;
    unsigned DstSho:2;
    unsigned DstLong:2;
    unsigned __padding3:18;
    unsigned BkpOfDstTrvld:21;
    unsigned BkpOfDstTrvld_UB:1;
    unsigned __padding4:2;
} __attribute__((packed));

// can_id = 0x1D0
struct CEMBodyFr28 {
    unsigned AmbTRawVal:11;
    unsigned AmbTRawQly:2;
    unsigned __padding0:19;
    unsigned VehSpdIndcdVal:9;
    unsigned VehSpdIndcdUnit:2;
    unsigned VehSpdIndcd_UB:1;
    unsigned AmbTRaw_UB:1;
    unsigned __padding1:2;
    unsigned AmbTIndcdWithUnit_UB:1;
    unsigned AmbTIndcd:12;
    unsigned AmbTIndcdQf:2;
    unsigned AmbTIndcdUnit:2;
} __attribute__((packed));

// can_id = 0x360
struct CEMBodyFr31 {
    unsigned VinPosn7:8;
    unsigned VinPosn6:8;
    unsigned VinPosn5:8;
    unsigned VinPosn4:8;
    unsigned VinPosn3:8;
    unsigned VinPosn2:8;
    unsigned VinPosn1:8;
    unsigned VinBlk:8;
} __attribute__((packed));

// can_id = 0x380
struct CEMBodyFr32 {
    unsigned __padding0:3;
    unsigned TrLockSts_UB:1;
    unsigned TrLockSts:2;
    unsigned __padding1:4;
    unsigned CurtReDirCmd:2;
    unsigned __padding2:18;
    unsigned CurtReDirCmd_UB:1;
    unsigned long long __padding3:33;
} __attribute__((packed));

// can_id = 0x330
struct CEMBodyFr33 {
    unsigned __padding0:21;
    unsigned VehBattUGroup_UB:1;
    unsigned VehBattUSysUQf:2;
    unsigned VehBattUSysU:8;
    unsigned __padding1:12;
    unsigned ProfPenSts1:4;
    unsigned TirePReRi:2;
    unsigned __padding2:2;
    unsigned TirePFrntRi:2;
    unsigned TirePFrntLe:2;
    unsigned __padding3:4;
    unsigned TirePReLe:2;
    unsigned __padding4:1;
    unsigned ProfPenSts1_UB:1;
} __attribute__((packed));

// can_id = 0x259
struct CEMBodyFr44 {
    unsigned long long __padding0:48;
    unsigned AmbTOutOfVehAmbTOutOfVeh:11;
    unsigned AmbTOutOfVehAmbTOutOfVehQf:2;
    unsigned AmbTOutOfVeh_UB:1;
    unsigned __padding1:2;
} __attribute__((packed));

// can_id = 0x95
struct POTBodyFr02 {
    unsigned long long __padding0:56;
    unsigned TrOpenerSts:4;
    unsigned TrOpenerSts_UB:1;
    unsigned __padding1:3;
} __attribute__((packed));

// can_id = 0x1E0
struct DDMBodyFr02 {
    unsigned __padding0:9;
    unsigned DrvWinPosnStsValue:7;
    unsigned DrvWinPosnStsAvailability:3;
    unsigned DrvWinPosnSts_UB:1;
    unsigned __padding1:12;
    unsigned WinPosnStsDrv:5;
    unsigned WinPosnStsDrv_UB:1;
    unsigned __padding2:12;
    unsigned AmbTRawAtDrvrSide_UB:1;
    unsigned AmbTQlyAtDrvrSide:2;
    unsigned AmbTValAtDrvrSide:11;
} __attribute__((packed));

// can_id = 0x130
struct DDMBodyFr03 {
    unsigned AmbTURawAtLeSideForObd:10;
    unsigned AmbTURawAtLeSideForObd_UB:1;
    unsigned long long __padding0:53;
} __attribute__((packed));

// can_id = 0x160
struct PDMBodyFr02 {
    unsigned __padding0:8;
    unsigned AmbTURawAtRiSideForObd:10;
    unsigned __padding1:4;
    unsigned AmbTURawAtRiSideForObd_UB:1;
    unsigned __padding2:14;
    unsigned WinPosnStsPass_UB:1;
    unsigned __padding3:5;
    unsigned WinPosnStsPass:5;
    unsigned AmbTValAtPassSide:11;
    unsigned AmbTQlyAtPassSide:2;
    unsigned AmbTRawAtPassSide_UB:1;
    unsigned __padding4:2;
} __attribute__((packed));

// can_id = 0xD0
struct VDDMChasFr04 {
    unsigned __padding0:9;
    unsigned PrpsnTqForBrkRels_UB:1;
    unsigned __padding1:22;
    unsigned PrpsnTqForBrkRels:15;
    unsigned __padding2:17;
} __attribute__((packed));

// can_id = 0xE0
struct VDDMChasFr05 {
    unsigned __padding0:2;
    unsigned VehSpdLgtSafeQf:2;
    unsigned VehSpdLgtSafeCntr:4;
    unsigned VehSpdLgtSafeChks:8;
    unsigned VehSpdLgtSafe:15;
    unsigned VehSpdLgtSafeGroup_UB:1;
    unsigned __padding1:1;
    unsigned DrvrBrkTqAtWhlsReqdGroup_UB:1;
    unsigned DrvrBrkTqAtWhlsReqdQf:2;
    unsigned DrvrBrkTqAtWhlsReqdDrvrBrkTqAtWhlsReqdCntr:4;
    unsigned DrvrBrkTqAtWhlsReqdDrvrBrkTqAtWhlsReqdChks:8;
    unsigned DrvrBrkTqAtWhlsReqd:16;
} __attribute__((packed));

// can_id = 0xF0
struct VDDMChasFr06 {
    unsigned TrSts_UB:1;
    unsigned __padding0:3;
    unsigned TrSts:2;
    unsigned HoodSts:2;
    unsigned DoorPassSts:2;
    unsigned DoorPassReSts:2;
    unsigned DoorDrvrSts:2;
    unsigned long long __padding1:44;
    unsigned HoodSts_UB:1;
    unsigned DoorPassSts_UB:1;
    unsigned DoorPassReSts_UB:1;
    unsigned DoorDrvrSts_UB:1;
    unsigned __padding2:2;
} __attribute__((packed));

// can_id = 0x3A0
struct VDDMChasFr08 {
    unsigned __padding0:13;
    unsigned DoorDrvrReSts_UB:1;
    unsigned DoorDrvrReSts:2;
    unsigned __padding1:16;
    unsigned AmbTIndcd:12;
    unsigned AmbTIndcdQf:2;
    unsigned AmbTIndcdUnit:2;
    unsigned __padding2:1;
    unsigned AmbTIndcdWithUnit_UB:1;
    unsigned __padding3:14;
} __attribute__((packed));

// can_id = 0x1C0
struct VDDMChasFr12 {
    unsigned long long __padding0:47;
    unsigned BrkPedlPsdSafeGroup_UB:1;
    unsigned BrkPedlNotPsdSafe:1;
    unsigned BrkPedlPsd:1;
    unsigned BrkPedlPsdQf:2;
    unsigned BrkPedlPsdCntr:4;
    unsigned BrkPedlPsdChks:8;
} __attribute__((packed));

// can_id = 0x1F0
struct VDDMChasFr16 {
    unsigned __padding0:11;
    unsigned DrvModReq_UB:1;
    unsigned __padding1:1;
    unsigned BkpOfDstTrvld_UB:1;
    unsigned __padding2:12;
    unsigned DrvModReq:4;
    unsigned __padding3:11;
    unsigned BkpOfDstTrvld:21;
} __attribute__((packed));

// can_id = 0x240
struct VDDMChasFr17 {
    unsigned __padding0:31;
    unsigned FuLvlIndcd_UB:1;
    unsigned __padding1:16;
    unsigned FuLvlIndcdVal:10;
    unsigned FuLvlIndcdQly:2;
    unsigned __padding2:4;
} __attribute__((packed));

// can_id = 0x2E0
struct VDDMChasFr19 {
    unsigned __padding0:8;
    unsigned VehMtnStSafeGroup_UB:1;
    unsigned VehMtnStSafe:3;
    unsigned VehMtnStCntr:4;
    unsigned VehMtnStChks:8;
    unsigned VehModMngtGlbSafe1_UB:1;
    unsigned __padding1:1;
    unsigned CarModSubtypWdCarModSubtyp:3;
    unsigned CarModSts1:3;
    unsigned EgyLvlElecMai:4;
    unsigned EgyLvlElecSubtyp:4;
    unsigned PwrLvlElecMai:4;
    unsigned PwrLvlElecSubtyp:4;
    unsigned UsgModSts:4;
    unsigned VehModMngtGlbSafe1Cntr:4;
    unsigned VehModMngtGlbSafe1Chks:8;
} __attribute__((packed));

// can_id = 0x380
struct VDDMChasFr25 {
    unsigned VehBattUSysU:8;
    unsigned VehBattUSysUQf:2;
    unsigned EngCooltLvl_UB:1;
    unsigned EngCooltLvl:1;
    unsigned __padding0:2;
    unsigned VehBattUGroup_UB:1;
    unsigned long long __padding1:49;
} __attribute__((packed));

// can_id = 0x1A
struct VDDMChasFr48 {
    unsigned BrkTqChks:8;
    unsigned BrkTqTotReqForPt:13;
    unsigned __padding0:3;
    unsigned BrkTqCntr:4;
    unsigned BrkTq_UB:1;
    unsigned long long __padding1:35;
} __attribute__((packed));

// can_id = 0x15
struct VDDMChasFr49 {
    unsigned WhlBrkPEstimdMax:8;
    unsigned BrkSysCylPMstChks:8;
    unsigned BrkSysCylPMstVirt:10;
    unsigned BrkSysCylPMstVirtQf:2;
    unsigned __padding0:4;
    unsigned BrkSysCylPMstTar:10;
    unsigned BrkSysCylPMstCntr:4;
    unsigned BrkSysCylPMstActQf:2;
    unsigned BrkSysCylPMstAct:10;
    unsigned BrkSysCylPMst_UB:1;
    unsigned WhlBrkPEstimdMax_UB:1;
    unsigned __padding1:4;
} __attribute__((packed));

// can_id = 0x35
struct ECMChasFr03 {
    unsigned __padding0:15;
    unsigned PrpsnTqReReq_UB:1;
    unsigned __padding1:16;
    unsigned AccrPedlRatGrdt_UB:1;
    unsigned AccrPedlRatGrdt:15;
    unsigned PrpsnTqReReq:16;
} __attribute__((packed));

// can_id = 0x100
struct ECMChasFr04 {
    unsigned __padding0:1;
    unsigned GearIndcnRec_UB:1;
    unsigned AccrPedlPsdAccrPedlPsdSts:1;
    unsigned AccrPedlPsdAccrPedlPsd:1;
    unsigned AccrPedlPsdAccrPedlPsdCntr:4;
    unsigned AccrPedlPsdAccrPedlPsdChks:8;
    unsigned long long __padding1:37;
    unsigned AccrPedlPsd_UB:1;
    unsigned GearShiftIndcn:2;
    unsigned GearTarIndcn:4;
    unsigned GearIndcn:4;
} __attribute__((packed));

// can_id = 0x110
struct ECMChasFr05 {
    unsigned __padding0:18;
    unsigned EngNGrdt1Safe_UB:1;
    unsigned __padding1:17;
    unsigned EngNGrdt1Cntr:4;
    unsigned EngNGrdt1Chks:8;
    unsigned EngNGrdt1:16;
} __attribute__((packed));

// can_id = 0x130
struct ECMChasFr06 {
    unsigned DrvrPrpsnTqReq:16;
    unsigned __padding0:2;
    unsigned EngNSafe_UB:1;
    unsigned DrvrPrpsnTqReq_UB:1;
    unsigned EngNCntr:4;
    unsigned EngNChks:8;
    unsigned EngN:15;
    unsigned __padding1:1;
    unsigned EngNSafeEngNGrdt:16;
} __attribute__((packed));

// can_id = 0x210
struct ECMChasFr10 {
    unsigned __padding0:30;
    unsigned CluPedlRat_UB:1;
    unsigned __padding1:9;
    unsigned CluPedlRat:8;
    unsigned __padding2:16;
} __attribute__((packed));

// can_id = 0x23A
struct ECMChasFr22 {
    unsigned __padding0:24;
    unsigned PrpsnTqDirChks:8;
    unsigned PrpsnTqDirCntr:4;
    unsigned PrpsnTqDirAct:2;
    unsigned PrpsnTqDir_UB:1;
    unsigned __padding1:25;
} __attribute__((packed));

#define MSG_BODY_0x40    CEMBodyFr02
#define MSG_BODY_0xB0    CEMBodyFr05
#define MSG_BODY_0xE0    CEMBodyFr11
#define MSG_BODY_0x100   CEMBodyFr13
#define MSG_BODY_0x1B0   CEMBodyFr17
#define MSG_BODY_0x250   CEMBodyFr23
#define MSG_BODY_0x270   CEMBodyFr25
#define MSG_BODY_0x300   CEMBodyFr26
#define MSG_BODY_0x310   CEMBodyFr27
#define MSG_BODY_0x1D0   CEMBodyFr28
#define MSG_BODY_0x360   CEMBodyFr31
#define MSG_BODY_0x380   CEMBodyFr32
#define MSG_BODY_0x330   CEMBodyFr33
#define MSG_BODY_0x259   CEMBodyFr44
#define MSG_BODY_0x95    POTBodyFr02
#define MSG_BODY_0x1E0   DDMBodyFr02
#define MSG_BODY_0x130   DDMBodyFr03
#define MSG_BODY_0x160   PDMBodyFr02

#define MSG_CHAS_0xD0    VDDMChasFr04
#define MSG_CHAS_0xE0    VDDMChasFr05
#define MSG_CHAS_0xF0    VDDMChasFr06
#define MSG_CHAS_0x3A0   VDDMChasFr08
#define MSG_CHAS_0x1C0   VDDMChasFr12
#define MSG_CHAS_0x1F0   VDDMChasFr16
#define MSG_CHAS_0x240   VDDMChasFr17
#define MSG_CHAS_0x2E0   VDDMChasFr19
#define MSG_CHAS_0x380   VDDMChasFr25
#define MSG_CHAS_0x1A    VDDMChasFr48
#define MSG_CHAS_0x15    VDDMChasFr49
#define MSG_CHAS_0x35    ECMChasFr03
#define MSG_CHAS_0x100   ECMChasFr04
#define MSG_CHAS_0x110   ECMChasFr05
#define MSG_CHAS_0x130   ECMChasFr06
#define MSG_CHAS_0x210   ECMChasFr10
#define MSG_CHAS_0x23A   ECMChasFr22


typedef union CanMsgs_BodyMessage {
    struct MSG_BODY_0x40 msg_0x40;
    struct MSG_BODY_0xB0 msg_0xB0;
    struct MSG_BODY_0xE0 msg_0xE0;
    struct MSG_BODY_0x100 msg_0x100;
    struct MSG_BODY_0x1B0 msg_0x1B0;
    struct MSG_BODY_0x250 msg_0x250;
    struct MSG_BODY_0x270 msg_0x270;
    struct MSG_BODY_0x300 msg_0x300;
    struct MSG_BODY_0x310 msg_0x310;
    struct MSG_BODY_0x1D0 msg_0x1D0;
    struct MSG_BODY_0x360 msg_0x360;
    struct MSG_BODY_0x380 msg_0x380;
    struct MSG_BODY_0x330 msg_0x330;
    struct MSG_BODY_0x259 msg_0x259;
    struct MSG_BODY_0x95 msg_0x95;
    struct MSG_BODY_0x1E0 msg_0x1E0;
    struct MSG_BODY_0x130 msg_0x130;
    struct MSG_BODY_0x160 msg_0x160;
} CanMsgs_BodyMessage_t;

typedef union CanMsgs_ChasMessage {
    struct MSG_CHAS_0xD0 msg_0xD0;
    struct MSG_CHAS_0xE0 msg_0xE0;
    struct MSG_CHAS_0xF0 msg_0xF0;
    struct MSG_CHAS_0x3A0 msg_0x3A0;
    struct MSG_CHAS_0x1C0 msg_0x1C0;
    struct MSG_CHAS_0x1F0 msg_0x1F0;
    struct MSG_CHAS_0x240 msg_0x240;
    struct MSG_CHAS_0x2E0 msg_0x2E0;
    struct MSG_CHAS_0x380 msg_0x380;
    struct MSG_CHAS_0x1A msg_0x1A;
    struct MSG_CHAS_0x15 msg_0x15;
    struct MSG_CHAS_0x35 msg_0x35;
    struct MSG_CHAS_0x100 msg_0x100;
    struct MSG_CHAS_0x110 msg_0x110;
    struct MSG_CHAS_0x130 msg_0x130;
    struct MSG_CHAS_0x210 msg_0x210;
    struct MSG_CHAS_0x23A msg_0x23A;
} CanMsgs_ChasMessage_t;

int CanMsgs_body_msg_supported(unsigned int msg_id);
int CanMsgs_chas_msg_supported(unsigned int msg_id);

#define CanMsgs_GET_MESSAGE(MSG_UNION, MSG_ID) (MSG_UNION.msg_##MSG_ID)

#endif
