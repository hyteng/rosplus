#include <iostream>
#include <iomanip>
#include "vmelib.h"
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <unistd.h>


using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::hex;
using std::dec;

#define MIN_IMAGE 0
#define MAX_IMAGE 7
#define MIN_MINOR 10
#define MAX_MINOR 17

// register inform
#define MQDC32_BASE                 0xDD000000
#define MQDC32_LENGTH               0x10000
#define MQDC32_FIFO_Offset          0x0000
#define MQDC32_Ch00Th_Offset        0x4000
#define MQDC32_Ch01Th_Offset        0x4002
#define MQDC32_Ch02Th_Offset        0x4004
#define MQDC32_Ch03Th_Offset        0x4006
#define MQDC32_Ch04Th_Offset        0x4008
#define MQDC32_Ch05Th_Offset        0x400A
#define MQDC32_Ch06Th_Offset        0x400C
#define MQDC32_Ch07Th_Offset        0x400E
#define MQDC32_Ch08Th_Offset        0x4010
#define MQDC32_Ch09Th_Offset        0x4012
#define MQDC32_Ch10Th_Offset        0x4014
#define MQDC32_Ch11Th_Offset       	0x4016
#define MQDC32_Ch12Th_Offset       	0x4018
#define MQDC32_Ch13Th_Offset       	0x401A
#define MQDC32_Ch14Th_Offset       	0x401C
#define MQDC32_Ch15Th_Offset       	0x401E
#define MQDC32_Ch16Th_Offset       	0x4020
#define MQDC32_Ch17Th_Offset       	0x4022
#define MQDC32_Ch18Th_Offset       	0x4024
#define MQDC32_Ch19Th_Offset       	0x4026
#define MQDC32_Ch20Th_Offset       	0x4028
#define MQDC32_Ch21Th_Offset       	0x402A
#define MQDC32_Ch22Th_Offset       	0x402C
#define MQDC32_Ch23Th_Offset       	0x402E
#define MQDC32_Ch24Th_Offset       	0x4030
#define MQDC32_Ch25Th_Offset       	0x4032
#define MQDC32_Ch26Th_Offset       	0x4034
#define MQDC32_Ch27Th_Offset       	0x4036
#define MQDC32_Ch28Th_Offset       	0x4038
#define MQDC32_Ch29Th_Offset       	0x403A
#define MQDC32_Ch30Th_Offset       	0x403C
#define MQDC32_Ch31Th_Offset       	0x403E
#define MQDC32_AddrSource_Offset   	0x6000
#define MQDC32_AddrReg_Offset      	0x6002
#define MQDC32_ModuleId_Offset     	0x6004
#define MQDC32_FastVme_Offset      	0x6006
#define MQDC32_SoftReset_Offset    	0x6008 //WO
#define MQDC32_Firmware_Offset     	0x600E //RO
#define MQDC32_IrqLevel_Offset     	0x6010
#define MQDC32_IrqVector_Offset    	0x6012
#define MQDC32_IrqTest_Offset      	0x6014 //WO
#define MQDC32_IrqReset_Offset     	0x6016 //WO
#define MQDC32_IrqTh_Offset        	0x6018
#define MQDC32_MaxTransfer_Offset  	0x601A
#define MQDC32_IrqWithdraw_Offset  	0x601C
#define MQDC32_BLTControl_Offset  	0x6020
#define MQDC32_CBLTAddr_Offset     	0x6022
#define MQDC32_MCSTAddr_Offset     	0x6024
#define MQDC32_DataLength_Offset   	0x6030 //RO
#define MQDC32_DataFormat_Offset   	0x6032
#define MQDC32_ReadoutReset_Offset 	0x6034 //WO
#define MQDC32_MultiEvent_Offset   	0x6036
#define MQDC32_MarkType_Offset     	0x6038
#define MQDC32_StartAcq_Offset     	0x603A
#define MQDC32_FIFOReset_Offset    	0x603C //WO
#define MQDC32_DataReady_Offset    	0x603E //RO
#define MQDC32_BankOpt_Offset      	0x6040
#define MQDC32_AdcRes_Offset       	0x6042 //fixed
#define MQDC32_ShiftBank0_Offset   	0x6044
#define MQDC32_ShiftBank1_Offset   	0x6046
#define MQDC32_SlideScale_Offset   	0x6048
#define MQDC32_OverFlow_Offset     	0x604A
#define MQDC32_DisableTh_Offset    	0x604C
#define MQDC32_LimitBank0_Offset   	0x6050
#define MQDC32_LimitBank1_Offset   	0x6052
#define MQDC32_ExpDelay0_Offset    	0x6054
#define MQDC32_ExpDelay1_Offset    	0x6056
#define MQDC32_InputCouple_Offset  	0x6060
#define MQDC32_ECLTerminator_Offset 0x6062
#define MQDC32_ECLGate1OSC_Offset  	0x6064
#define MQDC32_ECLFCReset_Offset   	0x6066
#define MQDC32_GateSelect_Offset   	0x6068
#define MQDC32_NIMGate1OSC_Offset  	0x606A
#define MQDC32_NIMFCReset_Offset   	0x606C
#define MQDC32_NIMBusy_Offset      	0x606E
#define MQDC32_PulserStatus_Offset 	0x6070
#define MQDC32_PulserDac_Offset    	0x6072
#define MQDC32_RCBusNo_Offset      	0x6080
#define MQDC32_RCModNum_Offset     	0x6082
#define MQDC32_RCOpCode_Offset     	0x6084
#define MQDC32_RCAddr_Offset       	0x6086
#define MQDC32_RCData_Offset       	0x6088
#define MQDC32_RCStatus_Offset     	0x608A //RO
#define MQDC32_CountReset_Offset   	0x6090
#define MQDC32_CTRALow_Offset      	0x6092
#define MQDC32_CTRAHigh_Offset     	0x6094
#define MQDC32_TSSourse_Offset     	0x6096
#define MQDC32_TSDivisor_Offset    	0x6098
#define MQDC32_TSLow_Offset        	0x609C //RO
#define MQDC32_TSHigh_Offset       	0x609E //RO
#define MQDC32_AdcTimeLow_Offset  	0x60A0 //RO
#define MQDC32_AdcTimeHigh_Offset 	0x60A2 //RO
#define MQDC32_Gate1TLow_Offset    	0x60A4 //RO
#define MQDC32_Gate1THigh_Offset   	0x60A6 //RO
#define MQDC32_Time0_Offset        	0x60A8 //RO
#define MQDC32_Time1_Offset        	0x60AA //RO
#define MQDC32_Time2_Offset        	0x60AC //RO
#define MQDC32_StopCTR_Offset      	0x60AE
#define MQDC32_Limit0High_Offset   	0x60B0
#define MQDC32_Limit0Low_Offset    	0x60B2
#define MQDC32_Limit1High_Offset   	0x60B4
#define MQDC32_Limit1Low_Offset    	0x60B6

#define MQDC32_Ch00Th_Mask			0x1FFF
#define MQDC32_Ch01Th_Mask			0x1FFF
#define MQDC32_Ch02Th_Mask			0x1FFF
#define MQDC32_Ch03Th_Mask			0x1FFF
#define MQDC32_Ch04Th_Mask			0x1FFF
#define MQDC32_Ch05Th_Mask			0x1FFF
#define MQDC32_Ch06Th_Mask			0x1FFF
#define MQDC32_Ch07Th_Mask			0x1FFF
#define MQDC32_Ch08Th_Mask			0x1FFF
#define MQDC32_Ch09Th_Mask			0x1FFF
#define MQDC32_Ch10Th_Mask			0x1FFF
#define MQDC32_Ch11Th_Mask			0x1FFF
#define MQDC32_Ch12Th_Mask			0x1FFF
#define MQDC32_Ch13Th_Mask			0x1FFF
#define MQDC32_Ch14Th_Mask			0x1FFF
#define MQDC32_Ch15Th_Mask			0x1FFF
#define MQDC32_Ch16Th_Mask			0x1FFF
#define MQDC32_Ch17Th_Mask			0x1FFF
#define MQDC32_Ch18Th_Mask			0x1FFF
#define MQDC32_Ch19Th_Mask			0x1FFF
#define MQDC32_Ch20Th_Mask			0x1FFF
#define MQDC32_Ch21Th_Mask			0x1FFF
#define MQDC32_Ch22Th_Mask			0x1FFF
#define MQDC32_Ch23Th_Mask			0x1FFF
#define MQDC32_Ch24Th_Mask			0x1FFF
#define MQDC32_Ch25Th_Mask			0x1FFF
#define MQDC32_Ch26Th_Mask			0x1FFF
#define MQDC32_Ch27Th_Mask			0x1FFF
#define MQDC32_Ch28Th_Mask			0x1FFF
#define MQDC32_Ch29Th_Mask			0x1FFF
#define MQDC32_Ch30Th_Mask			0x1FFF
#define MQDC32_Ch31Th_Mask			0x1FFF
#define MQDC32_AddrSource_Mask		0x0001
#define MQDC32_AddrReg_Mask        	0xFFFF
#define MQDC32_ModuleId_Mask		0x00FF
#define MQDC32_FastVme_Mask			0x0001
#define MQDC32_SoftReset_Mask		0x0001 //WO
#define MQDC32_Firmware_Mask		0xFFFF //RO
#define MQDC32_IrqLevel_Mask		0x0007
#define MQDC32_IrqVector_Mask	   	0x00FF
#define MQDC32_IrqTest_Mask	    	0x0000 //WO
#define MQDC32_IrqReset_Mask    	0x0000 //WO
#define MQDC32_IrqTh_Mask    		0xFFFF
#define MQDC32_MaxTransfer_Mask		0xFFFF
#define MQDC32_IrqWithdraw_Mask		0x0001
#define MQDC32_BLTControl_Mask		0x00FF
#define MQDC32_CBLTAddr_Mask		0x00FF
#define MQDC32_MCSTAddr_Mask		0x00FF
#define MQDC32_DataLength_Mask		0xFFFF //RO
#define MQDC32_DataFormat_Mask		0x0003
#define MQDC32_ReadoutReset_Mask	0x0000 //WO
#define MQDC32_MultiEvent_Mask		0x000F
#define MQDC32_MarkType_Mask		0x0003
#define MQDC32_StartAcq_Mask		0x0001
#define MQDC32_FIFOReset_Mask		0x0000 //WO
#define MQDC32_DataReady_Mask		0x0001 //RO
#define MQDC32_BankOpt_Mask			0x0007
#define MQDC32_AdcRes_Mask	    	0x0001 //fixed
#define MQDC32_ShiftBank0_Mask		0x00FF
#define MQDC32_ShiftBank1_Mask		0x00FF
#define MQDC32_SlideScale_Mask		0x0001
#define MQDC32_OverFlow_Mask		0x0001
#define MQDC32_DisableTh_Mask		0x0001
#define MQDC32_LimitBank0_Mask		0x00FF
#define MQDC32_LimitBank1_Mask		0x00FF
#define MQDC32_ExpDelay0_Mask		0x3FFF
#define MQDC32_ExpDelay1_Mask		0x3FFF
#define MQDC32_InputCouple_Mask		0x0007
#define MQDC32_ECLTerminator_Mask	0x001F
#define MQDC32_ECLGate1OSC_Mask		0x0001
#define MQDC32_ECLFCReset_Mask		0x0003
#define MQDC32_GateSelect_Mask		0x0001
#define MQDC32_NIMGate1OSC_Mask		0x0003
#define MQDC32_NIMFCReset_Mask		0x0003
#define MQDC32_NIMBusy_Mask			0x000F
#define MQDC32_PulserStatus_Mask	0x000F
#define MQDC32_PulserDac_Mask		0x00FF
#define MQDC32_RCBusNo_Mask			0x0003
#define MQDC32_RCModNum_Mask		0x000F
#define MQDC32_RCOpCode_Mask		0x007F
#define MQDC32_RCAddr_Mask	    	0x00FF
#define MQDC32_RCData_Mask		    0xFFFF
#define MQDC32_RCStatus_Mask		0x000F //RO
#define MQDC32_CountReset_Mask		0x000F
#define MQDC32_CTRALow_Mask			0xFFFF //RO
#define MQDC32_CTRAHigh_Mask		0xFFFF //RO
#define MQDC32_TSSourse_Mask		0x0003
#define MQDC32_TSDivisor_Mask		0xFFFF
#define MQDC32_TSLow_Mask	    	0xFFFF //RO
#define MQDC32_TSHigh_Mask	    	0xFFFF //RO
#define MQDC32_AdcTimeLow_Mask      0xFFFF //RO
#define MQDC32_AdcTimeHigh_Mask     0xFFFF //RO
#define MQDC32_Gate1TLow_Mask   	0xFFFF //RO
#define MQDC32_Gate1THigh_Mask		0xFFFF //RO
#define MQDC32_Time0_Mask	    	0xFFFF //RO
#define MQDC32_Time1_Mask		   	0xFFFF //RO
#define MQDC32_Time2_Mask		   	0xFFFF //RO
#define MQDC32_StopCTR_Mask			0x0003
#define MQDC32_Limit0High_Mask		0x003F
#define MQDC32_Limit0Low_Mask		0x003F
#define MQDC32_Limit1High_Mask		0x001F
#define MQDC32_Limit1Low_Mask		0x001F

#define EVENTSIZE 136

// ctrl, conf, and reg infomation

#define ctrlSize 112

static string ctrlMQDC32[ctrlSize] = {/*buf*/"buffer", /*ChTh*/"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", /*board*/"addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", /*irq*/"irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", /*blt*/"cbltCtrl", "mcstCtrl", "cbltAddr", "mcstAddr", /*fifo*/"dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", /*adc*/"bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", /*delay*/"limitBank0", "limitBank1", "expDelay0", "expDelay1", /*io*/"inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", /*pulser*/"pulserSt", "pulserDac", /*rc*/"rcBusNo", "rcModNum", "rcOpCode", "rcAddr", "rcSt", "rcData", /*ctra*/"ctraReset", "ctrbReset", "extReset", "eventCounter", "tsSource", "tsDivisor", "tsCounter", /*ctrb*/"adcTime", "gate1Time", "time0", "time1", "time2", "stopCounterB", "stopCounterA", /*limit*/"limit0High", "limit0Low", "limit1High", "limit1Low"};

static int ctrlSizeMQDC32[ctrlSize] = {/*buf*/1, /*ChTh*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*board*/1, 1, 1, 1, 1, 1, /*irq*/1, 1, 1, 1, 1, 1, 1, /*blt*/1, 1, 1, 1, /*fifo*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*adc*/1, 1, 1, 1, 1, 1, 1, 1, /*delay*/1, 1, 1, 1, /*io*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*pulser*/1, 1, /*rc*/1, 1, 1, 1, 1, 1, /*ctra*/1, 1, 1, 2, 1, 1, 2, /*ctrb*/2, 2, 1, 1, 1, 1, 1, /*limit*/1, 1, 1, 1}; // number of conf

static int ctrlRWMQDC32[ctrlSize] = {/*buf*/1, /*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0, 0, 0, 0, 2, 1, /*irq*/0, 0, 2, 2, 0, 0, 0, /*blt*/0, 0, 0, 1, /*fifo*/1, 0, 2, 0, 0, 0, 0, 0, 2, 1, /*adc*/0, 0, 0, 0, 0, 0, 0, 0, /*delay*/0, 0, 0, 0, /*io*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*pulser*/0, 0, /*rc*/0, 0, 0, 0, 0, 1, /*ctra*/0, 0, 0, 1, 0, 0, 1, /*ctrb*/1, 1, 1, 1, 1, 0, 0, /*limit*/1, 1, 1, 1}; // disable: -1, rw: 0, ro: 1, wo: 2

//#define confSize 116
#define confSetSize 76

enum MQDC32Conf {/*buf*/buffer=0, /*ChTh*/Ch00Th, Ch01Th, Ch02Th, Ch03Th, Ch04Th, Ch05Th, Ch06Th, Ch07Th, Ch08Th, Ch09Th, Ch10Th, Ch11Th, Ch12Th, Ch13Th, Ch14Th, Ch15Th, Ch16Th, Ch17Th, Ch18Th, Ch19Th, Ch20Th, Ch21Th, Ch22Th, Ch23Th, Ch24Th, Ch25Th, Ch26Th, Ch27Th, Ch28Th, Ch29Th, Ch30Th, Ch31Th, /*board*/addrSource, addrReg, moduleId, fastVme, softReset, firmware, /*irq*/irqLevel, irqVector, irqTest, irqReset, irqTh, maxTransfer, irqWithdraw, /*blt*/cbltCtrl, mcstctrl, cbltAddr, mcstAddr, /*fifo*/dataLength, dataFormat, readoutReset, multiEvent, skipBerr, countUnit, markType, startAcq, fifoReset, dataReady, /*adc*/bankConnect, switchPull0, switchPull1, shiftBank0, shiftBank1, slideScale, overFlow, disableTh, /*delay*/limitBank0, limitBank1, expDelay0, expDelay1, /*io*/inputCouple0, inputCouple1, BLR, terGate0, terGate1, terReset, terBank0, terBank1, eclGate1Osc, eclFCReset, gateSelect, nimGate1Osc, nimFCReset, nimBusy, /*pulser*/pulserSt, pulserDac, /*rc*/rcBusNo, rcModNum, rcOpCode, rcAddr, rcData, rcSt, /*ctra*/ctraReset, ctrbReset, extReset, eventCounterLow, eventCounterHigh, tsSource, tsDivisor, tsCounterLow, tsCounterHigh, /*ctrb*/adcTimeLow, adcTimeHigh, gate1TimeLow, gate1TimeHigh, time0, time1, time2, stopCounterB, stopCounterA, /*limit*/limit0High, limit0Low, limit1High, limit1Low, confSize};

static string confNameMQDC32[confSize] = {/*buf*/"buffer", /*ChTh*/"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", /*board*/"addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", /*irq*/"irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", /*blt*/"cbltCtrl", "mcstCtrl", "cbltAddr", "mcstAddr", /*fifo*/"dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", /*adc*/"bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", /*delay*/"limitBank0", "limitBank1", "expDelay0", "expDelay1", /*io*/"inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", /*pulser*/"pulserSt", "pulserDac", /*rc*/"rcBusNo", "rcModNum", "rcOpCode", "rcAddr", "rcData", "rcSt", /*ctra*/"ctraReset", "ctrbReset", "extReset", "eventCounterLow", "eventCounterHigh", "tsSource", "tsDivisor", "tsCounterLow", "tsCounterHigh", /*ctrb*/"adcTimeLow", "adcTimeHigh", "gate1TimeLow", "gate1TimeHigh", "time0", "time1", "time2", "stopCounterB", "stopCounterA", /*limit*/"limit0High", "limit0Low", "limit1High", "limit1Low"};

static uint16_t confMaskMQDC32[confSize] = {/*buf*/0xFFFF, /*ChTh*/0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, /*board*/0x0001, 0xFFFF, 0x00FF, 0x0001, 0x0001, 0xFFFF, /*irq*/0x0007, 0x00FF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0001, /*blt*/0x003F, 0x00C0, 0x00FF, 0x00FF, /*fifo*/0xFFFF, 0x0003, 0x0000, 0x0003, 0x0004, 0x0008, 0x0003, 0x0001, 0x0000, 0x0001, /*adc*/0x0001, 0x0002, 0x0004, 0x00FF, 0x00FF, 0x0001, 0x0001, 0x0001, /*delay*/0x00FF, 0x00FF, 0x3FFF, 0x3FFF, /*io*/0x0001, 0x0002, 0x0004, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0001, 0x0003, 0x0001, 0x0003, 0x0003, 0x000F, /*pluser*/0x000F, 0x00FF, /*rc*/0x0003, 0x000F, 0x007F, 0x00FF, 0xFFFF, 0x000F, /*ctra*/0x0001, 0x0002, 0x000C, 0xFFFF, 0xFFFF, 0x0003, 0xFFFF, 0xFFFF, 0xFFFF, /*ctrb*/0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0001, 0x0002, /*limit*/0x003F, 0x003F, 0x001F, 0x001F};

static int confDuplicMQDC32[14] = {47, 54, 55, 61, 62, 73, 74, 76, 77, 78, 79, 95, 96, 111};

static int confDefaultIdxMQDC32[confSetSize] = {/*ChTh*/1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, /*board*/35, 36, /*irq*/39, 40, 43, 44, /*blt*/46, 47, /*fifo*/53, 54, 55, 56, /*adc*/60, 61, 62, 65, 66, 67, /*delay*/68, 69, 70, 71, /*io*/72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, /*pulser*/86, 87, /*rc*//*ctra*/99, 100, /*ctrb*//*limit*/112, 113, 114, 115};

static uint16_t confDefaultValueMQDC32[confSetSize] = {/*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0xFF, 0, /*irq*/7, 0x00FE, 100, 4, /*blt*/0x0002, 0x0002, /*fifo*/3, 1, 1, 0, /*adc*/0, 0, 0, 0, 0, 0, /*delay*/255, 255, 0, 0, /*io*/0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, /*pulser*/0, 32, /*rc*//*ctra*/0, 0x0001, /*ctrb*//*limit*/32, 0, 16, 0};

#define regSize 102

static uint32_t regAddrMQDC32[regSize] = {/*buf*/0x0000, /*ChTh*/0x4000, 0x4002, 0x4004, 0x4006, 0x4008, 0x400A, 0x400C, 0x400E, 0x4010, 0x4012, 0x4014, 0x4016, 0x4018, 0x401A, 0x401C, 0x401E, 0x4020, 0x4022, 0x4024, 0x4026, 0x4028, 0x402A, 0x402C, 0x402E, 0x4030, 0x4032, 0x4034, 0x4036, 0x4038, 0x403A, 0x403C, 0x403E, /*board*/0x6000, 0x6002, 0x6004, 0x6006, 0x6008, 0x600E, /*irq*/0x6010, 0x6012, 0x6014, 0x6016, 0x6018, 0x601A, 0x601C, /*blt*/0x6020, 0x6022, 0x6024, /*fifo*/0x6030, 0x6032, 0x6034, 0x6036, 0x6038, 0x603A, 0x603C, 0x603E, /*adc*/0x6040, 0x6044, 0x6046, 0x6048, 0x604A, 0x604C, /*delay*/0x6050, 0x6052, 0x6054, 0x6056, /*io*/0x6060, 0x6062, 0x6064, 0x6066, 0x6068, 0x606A, 0x606C, 0x606E, /*pluser*/0x6070, 0x6072, /*rc*/0x6080, 0x6082, 0x6084, 0x6086, 0x6088, 0x608A, /*ctra*/0x6090, 0x6092, 0x6094, 0x6096, 0x6098, 0x609C, 0x609E, /*ctrb*/0x60A0, 0x60A2, 0x60A4, 0x60A6, 0x60A8, 0x60AA, 0x60AC, 0x60AE, /*limit*/0x60B0, 0x60B2, 0x60B4, 0x60B6};

static int regRWMQDC32[regSize] = {/*buf*/1, /*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0, 0, 0, 0, 2, 1, /*irq*/0, 0, 2, 2, 0, 0, 0, /*blt*/0, 0, 1, /*fifo*/1, 0, 2, 0, 0, 0, 2, 1, /*adc*/0, 0, 0, 0, 0, 0, /*delay*/0, 0, 0, 0, /*io*/0, 0, 0, 0, 0, 0, 0, 0, /*pulser*/0, 0, /*rc*/0, 0, 0, 0, 0, 1, /*ctra*/0, 1, 1, 0, 0, 1, 1, /*ctrb*/1, 1, 1, 1, 1, 1, 1, 0, /*limit*/1, 1, 1, 1}; // disable: -1, rw: 0, ro: 1, wo: 2

static map<string, vector<string> > mqdc32_ctrl2conf;
static map<string, int> mqdc32_conf2reg;
static map<string, uintptr_t> mqdc32_conf2mask;
static vector<uintptr_t> mqdc32_regAddr;
static vector<int> mqdc32_regRWIdx;

static int setCtrl() {
    mqdc32_ctrl2conf.clear();
    for(int i=0, j=0; i<ctrlSize; i++, j++) {
        mqdc32_ctrl2conf[ctrlMQDC32[i]] = vector<string>(0);
        for(int k=0; k<ctrlSizeMQDC32[i]; k++) {
            mqdc32_ctrl2conf[ctrlMQDC32[i]].push_back(confNameMQDC32[j]);
            j++;
        }
        j--;
    }

    mqdc32_conf2reg.clear();
    mqdc32_conf2mask.clear();
    for(int i=0, j=0; i<confSize; i++,j++) {
        if(find(confDuplicMQDC32, confDuplicMQDC32+confSize-regSize, i) != (confDuplicMQDC32+confSize-regSize))
            j--;
        mqdc32_conf2reg[confNameMQDC32[i]] = j;
        mqdc32_conf2mask[confNameMQDC32[i]] = (uintptr_t)(&confMaskMQDC32[i]);
        cout << hex << "mqdc32# " << "confName " << confNameMQDC32[i] << ", regAddr " << regAddrMQDC32[j] << dec << endl;
    }

    mqdc32_regAddr.clear();
    mqdc32_regRWIdx.clear();
    for(int i=0; i<regSize; i++) {
        mqdc32_regAddr.push_back((uintptr_t)(&regAddrMQDC32[i]));
        mqdc32_regRWIdx.push_back(regRWMQDC32[i]);
    }

    return 1;
}

static int dummy = setCtrl();

#define wordSize 4

int maskRegData(uint16_t& data, uint16_t& mask) {
    uint16_t mData = data;
    uint16_t mTest = mask;
    int shift = 16;
    for(int i=0;i<16;i++) {
        if((mTest>>i)%2 != 0) {
            shift = i;
            break;
        }
    }
    data = (mData<<shift)&mTest;
    return 1;
}

int main() {
    int image, op, i, j;
    uintptr_t adc_base, dma_base, fop, offset;
    uint32_t *ptr;
    uint16_t dummy16;
    uint16_t confValue[200], regValue[200];
    std::vector<int> regSet, confSet;
    VMEBridge vme;
    adc_base = MQDC32_BASE;

    vme.vmeSysReset();

    dma_base = vme.requestDMA();
    if (!dma_base) {
        cerr << "Can't allocate DMA !\n";
        return 0;
    }
    vme.setOption(DMA, BLT_ON);

    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

    if(vme.readUniReg(0x004)&0xF9000000) {
        vme.writeUniReg(0x004, 0xF9000007);
        //cin >> dummy32;
        cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;
    }

    image = vme.getImage(adc_base, MQDC32_LENGTH, A32, D16, MASTER);
    if (image < 0 || image > 7) {
        cerr << "Can't allocate master image !\n";
        return 0;
    }

    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

    //if((vmectl&0x00080000)>>19 == 1)
    //vme.writeUniReg(0x400, vmectl&0xFFFBFFFF);
    //cin >> dummy32;

    uint32_t confTemp;
    uint16_t data, mask;
    regSet.clear();
    for(int i=0;i<200;i++) {
        confValue[i] = 0;
        regValue[i] = 0;
    }
    for(int i=0; i<confSetSize; i++) {
        int confIdx = confDefaultIdxMQDC32[i];
        confValue[confIdx] = confDefaultValueMQDC32[i];
        int regIdx = mqdc32_conf2reg[confNameMQDC32[confIdx]];
        data = confValue[confIdx];
        mask = confMaskMQDC32[confIdx];
        maskRegData(data, mask);
        regValue[regIdx] = regValue[regIdx] & (~confMaskMQDC32[confIdx]) | data;
        cout << hex << "mqdc32# " << "regIdx " << regIdx << ", conf " << data << ", mask " << mask << ", regValue " << regValue[regIdx] << endl;
        if(find(regSet.begin(), regSet.end(), regIdx) == regSet.end())
            regSet.push_back(regIdx);
    }

    // stop adc
    vme.ww(image, adc_base+MQDC32_StartAcq_Offset, vme.swap16((uint16_t)0x0000));
    // clear data
    //vme.ww(image, adc_base+MQDC32_FIFOReset_Offset, vme.swap16((uint16_t)0x0001));
    // clear Irq and Berr
    //vme.ww(image, adc_base+MQDC32_ReadoutReset_Offset, vme.swap16((uint16_t)0x0001));

    // irq status
    uint16_t irqST;
    vme.rw(image, adc_base+0x6012, &irqST);
    irqST = vme.swap16(irqST);
    cout << "mqdc32# " << "irqST: " << irqST << endl;
    // irq vector
    uint16_t irqVT;
    vme.rw(image, adc_base+0x6010, &irqVT);
    irqVT = vme.swap16(irqVT);
    cout << "mqdc32# " << "irqST: " << irqVT << endl;

    int rw = 1;
    uint16_t testReg;
    for(unsigned int i=0; i<regSet.size(); i++) {
        data = regValue[regSet[i]];
        cout << hex << "mqdc32# " << "regIdx " << regSet[i] << ", data " << data << endl;
        vme.ww(image, adc_base+(*(uint16_t*)(mqdc32_regAddr[regSet[i]])), vme.swap16(data));
        cout << hex << "mqdc# " << "regAddr " << *(uint16_t*)(mqdc32_regAddr[regSet[i]]) << endl;
        vme.rw(image, adc_base+(*(uint16_t*)(mqdc32_regAddr[regSet[i]])), &testReg);
        cout << hex << "mqdc# " << "regIdx " << regSet[i] << ", read " << vme.swap16(testReg) << endl;
    }

    // setup vme irq
    if(confValue[irqLevel]>0 && confValue[irqLevel]<=7)
        vme.setupIrq(image, confValue[irqLevel], confValue[irqVector], 0, 0, 0, 0);
    cout << hex << "mqdc# setup vme irq: " << confValue[irqLevel] << ", " << confValue[irqVector] << endl;
    cout << hex << "mqdc# configAdc done." << endl;
    /*
    cin >> hex >> op;
    cin.sync();
    do {
        if(op < 0 && (offset <= 0x10BC || (offset >= 0x8000 && offset <= 0xFFFE)) && offset >= 0x0) {
            dummy16 = 0x0;
            vme.rw(image, adc_base+offset, &dummy16);
            cout << hex << "ADC: read 0x" << offset << ", value 0x" << dummy16 << dec << "!\n";
            op = 0;
        }
        if(op > 0 && offset <= 0x10BC && offset >= 0x0) {
            cout << hex << "word for adc register 0x" << offset << " : " << dec;
            cin >> hex >> dummy16;
            cin.sync();
            cout <<hex << "write 0x" << dummy16 << endl;
            vme.ww(image, adc_base+offset, &dummy16);
            op = 0;
        }
        cout << "manual operation" << endl;
        cout << "offset for adc register: ";
        cin >> hex >> offset;
        cin.sync();
        cout << "0 for quit; positive value for write; nagative value for read; what is your choice: ";
        cin >> hex >> op;
        cin.sync();
    } while(op != 0);
    */

    // stop adc
    vme.ww(image, adc_base+MQDC32_StartAcq_Offset, vme.swap16((uint16_t)0x0000));
    // clear data
    vme.ww(image, adc_base+MQDC32_FIFOReset_Offset, vme.swap16((uint16_t)0x0001));
    sleep(1);
    cout << "clear fifo" << endl;
    // clear Irq and Berr
    vme.ww(image, adc_base+MQDC32_ReadoutReset_Offset, vme.swap16((uint16_t)0x0001));
    sleep(1);
    cout << "reset readout" << endl;
    // start adc
    vme.ww(image, adc_base+MQDC32_StartAcq_Offset, vme.swap16((uint16_t)0x0001));
    sleep(1);
    cout << "waiting irq: " << confValue[irqLevel] << ", " << confValue[irqVector] << endl;
    vme.waitIrq(confValue[irqLevel], confValue[irqVector]);

    cout << "reading data..." << endl;
    //vme.generateVmeIrq(1, 0);
    //vme.generateVmeIrq(2, 0);
    //vme.generateVmeIrq(3, 0);
    //vme.generateVmeIrq(4, 0);
    //vme.generateVmeIrq(5, 0);
    //vme.generateVmeIrq(6, 0);
    //vme.generateVmeIrq(7, 0);
    uint16_t dataLength = 0;
    string cmd;
    while(true) {
        vme.rw(image, adc_base+0x6030, &dataLength);
        dataLength = vme.swap16(dataLength);
        cout << "data length " << dec << dataLength << hex << endl;
        cin.sync();
        cin >> cmd;
        if(cmd == "quit")
            break;
    }
    offset = -1;
    if(dataLength > 0)
        offset = vme.DMAread(adc_base, dataLength*4, A32, D32);
    if (offset < 0) {
        vme.releaseDMA();
        return 0;
    }
    // clear Irq and Berr
    cout << "acknowledge irq" << endl;
    vme.ww(image, adc_base+MQDC32_ReadoutReset_Offset, vme.swap16((uint16_t)0x0001));

    ptr = (uint32_t*)(dma_base+offset);
    for(i = 0x0; i < dataLength; i++) {
        if(i%8 == 0)
            cout << hex << "Buffer " << setw(2) << setfill('0') << i/7 << dec << ":   ";
        cout << hex << "0x" << setw(8) << setfill('0') << *ptr++ << dec << " ";
        if(i%8 == 7)
            cout << endl;                                                
    }
 
    vme.freeIrq(image, confValue[irqLevel], confValue[irqVector]);   
    vme.releaseDMA();

    cout << "operation finish and release adc device" << endl;
    vme.releaseImage(image);
    //cin >> dummy32;
    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

}
