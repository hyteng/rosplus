#include "mqdc32.h"
#include <algorithm>

using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::cout;
using std::endl;

#define MIN_IMAGE 0
#define MAX_IMAGE 7
#define MIN_MINOR 10
#define MAX_MINOR 17

// register inform
#define MQDC32_BASE                 0xCC000000
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


// ctrl, conf, and reg infomation

#define ctrlSize 111

static string ctrlMQDC32[ctrlSize] = {/*buf*/"buffer", /*ChTh*/"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", /*board*/"addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", /*irq*/"irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", /*blt*/"bltCtrl", "cbltAddr", "mcstAddr", /*fifo*/"dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", /*adc*/"bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", /*delay*/"limitBank0", "limitBank1", "expDelay0", "expDelay1", /*io*/"inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", /*pulser*/"pulserSt", "pulserDac", /*rc*/"rcBusNo", "rcModNum", "rcOpCode", "rcAddr", "rcSt", "rcData", /*ctra*/"ctraReset", "ctrbReset", "extReset", "eventCounter", "tsSource", "tsDivisor", "tsCounter", /*ctrb*/"adcTime", "gate1Time", "time0", "time1", "time2", "stopCounterB", "stopCounterA", /*limit*/"limit0High", "limit0Low", "limit1High", "limit1Low"};

static int ctrlSizeMQDC32[ctrlSize] = {/*buf*/1, /*ChTh*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*board*/1, 1, 1, 1, 1, 1, /*irq*/1, 1, 1, 1, 1, 1, 1, /*blt*/1, 1, 1, /*fifo*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*adc*/1, 1, 1, 1, 1, 1, 1, 1, /*delay*/1, 1, 1, 1, /*io*/1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*pulser*/1, 1, /*rc*/1, 1, 1, 1, 1, 1, /*ctra*/1, 1, 1, 2, 1, 1, 2, /*ctrb*/2, 2, 1, 1, 1, 1, 1, /*limit*/1, 1, 1, 1}; // number of conf

static int ctrlRWMQDC32[ctrlSize] = {/*buf*/1, /*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0, 0, 0, 0, 2, 1, /*irq*/0, 0, 2, 2, 0, 0, 0, /*blt*/0, 0, 1, /*fifo*/1, 0, 2, 0, 0, 0, 0, 0, 2, 1, /*adc*/0, 0, 0, 0, 0, 0, 0, 0, /*delay*/0, 0, 0, 0, /*io*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*pulser*/0, 0, /*rc*/0, 0, 0, 0, 0, 1, /*ctra*/0, 0, 0, 1, 0, 0, 1, /*ctrb*/1, 1, 1, 1, 1, 0, 0, /*limit*/1, 1, 1, 1}; // disable: -1, rw: 0, ro: 1, wo: 2

#define confSize 115

static string confNameMQDC32[confSize] = {/*buf*/"buffer", /*ChTh*/"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", /*board*/"addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", /*irq*/"irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", /*blt*/"bltCtrl", "cbltAddr", "mcstAddr", /*fifo*/"dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", /*adc*/"bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", /*delay*/"limitBank0", "limitBank1", "expDelay0", "expDelay1", /*io*/"inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", /*pulser*/"pulserSt", "pulserDac", /*rc*/"rcBusNo", "rcModNum", "rcOpCode", "rcAddr", "rcData", "rcSt", /*ctra*/"ctraReset", "ctrbReset", "extReset", "eventCounterLow", "eventCounterHigh", "tsSource", "tsDivisor", "tsCounterLow", "tsCounterHigh", /*ctrb*/"adcTimeLow", "adcTimeHigh", "gate1TimeLow", "gate1TimeHigh", "time0", "time1", "time2", "stopCounterB", "stopCounterA", /*limit*/"limit0High", "limit0Low", "limit1High", "limit1Low"};

static uint16_t confMaskMQDC32[confSize] = {/*buf*/0xFFFF, /*ChTh*/0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, 0x1FFF, /*board*/0x0001, 0xFFFF, 0x00FF, 0x0001, 0x0001, 0xFFFF, /*irq*/0x0007, 0x00FF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0001, /*blt*/0x00FF, 0x00FF, 0x00FF, /*fifo*/0xFFFF, 0x0003, 0x0000, 0x0003, 0x0004, 0x0008, 0x0003, 0x0001, 0x0000, 0x0001, /*adc*/0x0001, 0x0002, 0x0004, 0x00FF, 0x00FF, 0x0001, 0x0001, 0x0001, /*delay*/0x00FF, 0x00FF, 0x3FFF, 0x3FFF, /*io*/0x0001, 0x0002, 0x0004, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0001, 0x0003, 0x0001, 0x0003, 0x0003, 0x000F, /*pluser*/0x000F, 0x00FF, /*rc*/0x0003, 0x000F, 0x007F, 0x00FF, 0xFFFF, 0x000F, /*ctra*/0x0001, 0x0002, 0x000C, 0xFFFF, 0xFFFF, 0x0003, 0xFFFF, 0xFFFF, 0xFFFF, /*ctrb*/0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0001, 0x0002, /*limit*/0x003F, 0x003F, 0x001F, 0x001F};

static int confDupMQDC32[confSize-regSize] = {53, 54, 60, 61, 72, 73, 75, 76, 77, 78, 94, 95, 111};

static unsigned int confDefault[]={};

#define regSize 102

uint32_t regAddrMQDC32[regSize] = {/*buf*/0x0000, /*ChTh*/0x4000, 0x4002, 0x4004, 0x4006, 0x4008, 0x400A, 0x400C, 0x400E, 0x4010, 0x4012, 0x4014, 0x4016, 0x4018, 0x401A, 0x401C, 0x401E, 0x4020, 0x4022, 0x4024, 0x4026, 0x4028, 0x402A, 0x402C, 0x402E, 0x4030, 0x4032, 0x4034, 0x4036, 0x4038, 0x403A, 0x403C, 0x403E, /*board*/0x6000, 0x6002, 0x6004, 0x6006, 0x6008, 0x600E, /*irq*/0x6010, 0x6012, 0x6014, 0x6016, 0x6018, 0x601A, 0x601C, /*blt*/0x6020, 0x6022, 0x6024, /*fifo*/0x6030, 0x6032, 0x6034, 0x6036, 0x6038, 0x603A, 0x603C, 0x603E, /*adc*/0x6040, 0x6044, 0x6046, 0x6048, 0x604A, 0x604C, /*delay*/0x6050, 0x6052, 0x6054, 0x6056, /*io*/0x6060, 0x6062, 0x6064, 0x6066, 0x6068, 0x606A, 0x606C, 0x606E, /*pluser*/0x6070, 0x6072, /*rc*/0x6080, 0x6082, 0x6084, 0x6086, 0x6088, 0x608A, /*ctra*/0x6090, 0x6092, 0x6094, 0x6096, 0x6098, 0x609C, 0x609E, /*ctrb*/0x60A0, 0x60A2, 0x60A4, 0x60A6, 0x60A8, 0x60AA, 0x60AC, 0x60AE, /*limit*/0x60B0, 0x60B2, 0x60B4, 0x60B6};

static int regRWMQDC32[regSize] = {/*buf*/1, /*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0, 0, 0, 0, 2, 1, /*irq*/0, 0, 2, 2, 0, 0, 0, /*blt*/0, 0, 1, /*fifo*/1, 0, 2, 0, 0, 0, 2, 1, /*adc*/0, 0, 0, 0, 0, 0, /*delay*/0, 0, 0, 0, /*io*/0, 0, 0, 0, 0, 0, 0, 0, /*pulser*/0, 0, /*rc*/0, 0, 0, 0, 0, 1, /*ctra*/0, 1, 1, 0, 0, 1, 1, /*ctrb*/1, 1, 1, 1, 1, 1, 1, 0, /*limit*/1, 1, 1, 1}; // disable: -1, rw: 0, ro: 1, wo: 2

static map<string, vector<string> > mqdc32_ctrl2conf;
static map<string, int> mqdc32_conf2reg;
static map<string, uintptr_t>mqdc32_conf2mask;
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
    }

    mqdc32_conf2reg.clear();
    mqdc32_conf2mask.clear();
    for(int i=0, j=0; i<confSize; i++,j++) {
        if(find(confDupMQDC32, confDupMQDC32+confSize-regSize-1, i) != (confDupMQDC32+confSize-regSize-1))
            j--;
        mqdc32_conf2reg[confNameMQDC32[i]] = j;
        mqdc32_conf2mask[confNameMQDC32[i]] = (uintptr_t)(&confMaskMQDC32[i]);
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


mqdc32::mqdc32(const string& n): smBase(n) {
    ctrl2conf = &mqdc32_ctrl2conf;
    conf2reg = &mqdc32_conf2reg;
    conf2mask = &mqdc32_conf2mask;
    regAddr = &mqdc32_regAddr;
    regRWIdx = &mqdc32_regRWIdx;

    vd = (regData*) new regUint16();
    vm = (regData*) new regUint16();
}

mqdc32::~mqdc32() {
}

int mqdc32::InitializedLOAD(void* argv[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return 2;
}

int mqdc32::LoadedUNLD(void* argv[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return 1;
}

int mqdc32::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //if(!configAdc())
        //return -1;
    return 3;
}

int mqdc32::ConfiguredUNCF(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseAdc();
    return 2;
}

int mqdc32::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    //prepAdc();
    return 4;
}

int mqdc32::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    //finishAdc();
    return 3;
}

int mqdc32::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    //startAdc();
    return 5;
}

int mqdc32::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int mqdc32::RunningPAUS(void* argv[]) {
    debugMsg << name << "# " << "RunningPAUS";
    stMsg->stateOut(debugMsg);
    //disableAdc();
    return 6;
}

int mqdc32::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int mqdc32::PausedRESU(void* argv[]) {
    debugMsg << name << "# " << "PausedRESU";
    stMsg->stateOut(debugMsg);
    //enableAdc();
    return 5;
}

int mqdc32::accessReg(const int idx, const int rw, regData& data) {
    int res;
    if(idx<0 || idx>=regSize || rw<0 || rw>1 || data.getValueP()==NULL )
        return 0;

    if((find(regROIdx->begin(), regROIdx->end(), idx) != regROIdx->end()) && rw==1)
        return 0;
    if((find(regWOIdx->begin(), regWOIdx->end(), idx) != regWOIdx->end()) && rw==0)
        return 0;

    regAddrType addr = *(regAddrType*)((*regAddr)[idx]);
    
    res = accessRegNormal(addr, rw, (regType*)data.getValueP());
    return res;
}

int mqdc32::maskRegData(regData& data, regData& mask) {
    regType mData = *(regType*)(data.getValueP());
    regType mTest = *(regType*)(mask.getValueP());
    int shift = 16;
    for(int i=0;i<16;i++) {
        if((mTest>>i)%2 != 0) {
            shift = i;
            break;
        }
    }
    mData = (mData<<shift)&mTest;
    data.setValueP(&mData);
    return 1;
}

int mqdc32::unmaskRegData(regData& data, regData& mask) {
    regType mData = *(regType*)(data.getValueP());
    regType mTest = *(regType*)(mask.getValueP());
    int shift = 16;
    for(int i=0;i<16;i++) {
        if((mTest>>i)%2 != 0) {
            shift = i;
            break;
        }
    }
    mData = (mData&mTest)>>shift;
    data.setValueP(&mData);
    return 1;
}

int mqdc32::configAdc() {
    int res;

    pvme = NULL;
    string vmeModeName;
    if((res=cfgInfo->infoGetString("config."+name+".vmeModeName", vmeModeName)) == 1) {
        std::vector< std::pair<std::string, smBase*> >::const_iterator iter;
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == vmeModeName)
                pvme = (VMEBridge*)(iter->second->getHelp());
        }
    }
    if(pvme == NULL) {
        debugMsg << name << "# " << "helper " << vmeModeName << " not found.";
        stMsg->stateOut(debugMsg);
        return 0;
    }

    base = ((confDefault1785[AddrHigh]&ADC1785_AddrHigh_Mask)<<24) + ((regValue[AddrLow]&ADC1785_AddrLow_Mask)<<16);
    if((res=cfgInfo->infoGetUint("config."+name+".base", base)) != 1) {
        debugMsg << name << "# " << "config."+name+".base" << " not found.";
        stMsg->stateOut(debugMsg);
        //return 0;
    }
    length = ADC1785_LENGTH;
    if((res=cfgInfo->infoGetUint("config."+name+".length", length)) != 1) {
        debugMsg << name << "# " << "config."+name+".length" << " not found.";
        stMsg->stateOut(debugMsg);
        //return 0;
    }

    int i;
    for(i=0; i<ConfSize-ConfRO; i++)
        confValue[i]=confDefault1785[i];
    
    debugMsg << name << "# " << "base: " << hex << base << ", length: " << hex << length;
    stMsg->stateOut(debugMsg);
    image = pvme->getImage(base, length, A32, D16, MASTER);
    if(image < MIN_IMAGE || image > MAX_IMAGE)
        return 0;

    uint16_t regBitSet1, regBitSet2, regCtrl1, regStatus1, regStatus2;
    pvme->rw(image, base+ADC1785_BitSet1_Offset, &regBitSet1);
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    pvme->rw(image, base+ADC1785_Ctrl1_Offset, &regCtrl1);
    pvme->rw(image, base+ADC1785_Status1_Offset, &regStatus1);
    pvme->rw(image, base+ADC1785_Status2_Offset, &regStatus2);
    regBitSet1 = pvme->swap16(regBitSet1);
    regBitSet2 = pvme->swap16(regBitSet2);
    regCtrl1 = pvme->swap16(regCtrl1);
    regStatus1 = pvme->swap16(regStatus1);
    regStatus2 = pvme->swap16(regStatus2);
    debugMsg << name << "# " << hex << "BitSet1: " << regBitSet1 << ", BitSet2: " << regBitSet2 << ", Ctrl1: " << regCtrl1 << ", Status1: " << regStatus1 << ", Status2: " << regStatus2;
    stMsg->stateOut(debugMsg);

    unsigned int adcReg;
    for(i=0; i<ConfSize-ConfRO; i++) {
        if((res=cfgInfo->infoGetUint("config."+name+"."+confName1785[i], adcReg)) != 1) {
            debugMsg << name << "# " << "config."+name+"."+confName1785[i] << " not found.";
            stMsg->stateOut(debugMsg);
            //return 0;
        }
        else
            confValue[i] = adcReg;

        debugMsg << name << "# " << "" << confName1785[i] << ": " << confValue[i];
        stMsg->stateOut(debugMsg);
    }

    // merge to register
    for(i=0; i<RegSize-RegRO-RegBitSet; i++) {
        regValue[i] = confValue[i];
    }
    // BitSet1
    regValue[BitSet1] = ((confValue[berrFlag]&0x0001)<<3)+((confValue[selAddr]&0x0001)<<4)+((confValue[softReset]&0x0001)<<7);
    // Ctrl1
    regValue[Ctrl1] = ((confValue[blkend]&0x0001)<<2)+((confValue[progResetMod]&0x0001)<<4)+((confValue[busError]&0x0001)<<5)+((confValue[align64]&0x0001)<<6);
    // BitSet2
    regValue[BitSet2] = ((confValue[mode]&0x0001)<<0)+((confValue[offline]&0x0001)<<1)+((confValue[clearData]&0x0001)<<2)+((confValue[overRange]&0x0001)<<3)+((confValue[lowTh]&0x0001)<<4)+((confValue[testACQ]&0x0001)<<6)+((confValue[slideScale]&0x0001)<<7)+((confValue[stepTh]&0x0001)<<8)+((confValue[autoInc]&0x0001)<<11)+((confValue[emptyProg]&0x0001)<<12)+((confValue[slideSub]&0x0001)<<13)+((confValue[allTrigger]&0x0001)<<14);
    debugMsg << name << "# " << "regValue# " << hex << "BitSet1: " << regValue[BitSet1] << ", Ctrl1: " << regValue[Ctrl1] << ", BitSet2: " << regValue[BitSet2];
    stMsg->stateOut(debugMsg);

    debugMsg << name << "# " << hex << "IrqLevel: " << regValue[IrqLevel] << ", IrqVector: " << regValue[IrqVector];
    stMsg->stateOut(debugMsg);
    // set normal register value
    pvme->ww(image, base+ADC1785_IrqLevel_Offset, pvme->swap16(regValue[IrqLevel]&ADC1785_IrqLevel_Mask));
    pvme->ww(image, base+ADC1785_IrqVector_Offset, pvme->swap16(regValue[IrqVector]&ADC1785_IrqVector_Mask));
    pvme->ww(image, base+ADC1785_AddrHigh_Offset, pvme->swap16(regValue[AddrHigh]&ADC1785_AddrHigh_Mask));
    pvme->ww(image, base+ADC1785_AddrLow_Offset, pvme->swap16(regValue[AddrLow]&ADC1785_AddrLow_Mask));
    pvme->ww(image, base+ADC1785_EventTrig_Offset, pvme->swap16(regValue[EventTrig]&ADC1785_EventTrig_Mask));
    pvme->ww(image, base+ADC1785_FCLRWindow_Offset, pvme->swap16(regValue[FCLRWindow]&ADC1785_FCLRWindow_Mask));
    pvme->ww(image, base+ADC1785_CrateSelect_Offset, pvme->swap16(regValue[CrateSelect]&ADC1785_CrateSelect_Mask));
    pvme->ww(image, base+ADC1785_SlideConst_Offset, pvme->swap16(regValue[SlideConst]&ADC1785_SlideConst_Mask));
    // set Threshold
    pvme->ww(image, base+ADC1785_ThCh0H_Offset, pvme->swap16(regValue[ADC1785_ThCh0H]&ADC1785_ThCh0H_Mask));
    pvme->ww(image, base+ADC1785_ThCh0L_Offset, pvme->swap16(regValue[ADC1785_ThCh0L]&ADC1785_ThCh0L_Mask));
    pvme->ww(image, base+ADC1785_ThCh1H_Offset, pvme->swap16(regValue[ADC1785_ThCh1H]&ADC1785_ThCh1H_Mask));
    pvme->ww(image, base+ADC1785_ThCh1L_Offset, pvme->swap16(regValue[ADC1785_ThCh1L]&ADC1785_ThCh1L_Mask));
    pvme->ww(image, base+ADC1785_ThCh2H_Offset, pvme->swap16(regValue[ADC1785_ThCh2H]&ADC1785_ThCh2H_Mask));
    pvme->ww(image, base+ADC1785_ThCh2L_Offset, pvme->swap16(regValue[ADC1785_ThCh2L]&ADC1785_ThCh2L_Mask));
    pvme->ww(image, base+ADC1785_ThCh3H_Offset, pvme->swap16(regValue[ADC1785_ThCh3H]&ADC1785_ThCh3H_Mask));
    pvme->ww(image, base+ADC1785_ThCh3L_Offset, pvme->swap16(regValue[ADC1785_ThCh3L]&ADC1785_ThCh3L_Mask));
    pvme->ww(image, base+ADC1785_ThCh4H_Offset, pvme->swap16(regValue[ADC1785_ThCh4H]&ADC1785_ThCh4H_Mask));
    pvme->ww(image, base+ADC1785_ThCh4L_Offset, pvme->swap16(regValue[ADC1785_ThCh4L]&ADC1785_ThCh4L_Mask));
    pvme->ww(image, base+ADC1785_ThCh5H_Offset, pvme->swap16(regValue[ADC1785_ThCh5H]&ADC1785_ThCh5H_Mask));
    pvme->ww(image, base+ADC1785_ThCh5L_Offset, pvme->swap16(regValue[ADC1785_ThCh5L]&ADC1785_ThCh5L_Mask));
    pvme->ww(image, base+ADC1785_ThCh6H_Offset, pvme->swap16(regValue[ADC1785_ThCh6H]&ADC1785_ThCh6H_Mask));
    pvme->ww(image, base+ADC1785_ThCh6L_Offset, pvme->swap16(regValue[ADC1785_ThCh6L]&ADC1785_ThCh6L_Mask));
    pvme->ww(image, base+ADC1785_ThCh7H_Offset, pvme->swap16(regValue[ADC1785_ThCh7H]&ADC1785_ThCh7H_Mask));
    pvme->ww(image, base+ADC1785_ThCh7L_Offset, pvme->swap16(regValue[ADC1785_ThCh7L]&ADC1785_ThCh7L_Mask));
    
    // set Irq
    pvme->setupIrq(image, confValue[irqLevel], confValue[irqVector], 0, 0, 0, 0);

    // set BitSet1
    pvme->ww(image, base+ADC1785_BitSet1_Offset, pvme->swap16(regValue[BitSet1]&ADC1785_BitSet1_Mask));
    if(((regValue[BitSet1]&ADC1785_BitSet1_Mask)^ADC1785_BitSet1_Mask) != 0)
        pvme->ww(image, base+ADC1785_BitClear1_Offset, pvme->swap16((regValue[BitSet1]&ADC1785_BitSet1_Mask)^ADC1785_BitSet1_Mask));
    // set Ctrl1
    pvme->ww(image, base+ADC1785_Ctrl1_Offset, pvme->swap16(regValue[Ctrl1]&ADC1785_Ctrl1_Mask));
    // set BitSet2
    pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16(regValue[BitSet2]&ADC1785_BitSet2_Mask));
    if(((regValue[BitSet2]&ADC1785_BitSet2_Mask)^ADC1785_BitSet2_Mask) != 0)
        pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((regValue[BitSet2]&ADC1785_BitSet2_Mask)^ADC1785_BitSet2_Mask));

    pvme->rw(image, base+ADC1785_BitSet1_Offset, &regBitSet1);
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    pvme->rw(image, base+ADC1785_Ctrl1_Offset, &regCtrl1);
    pvme->rw(image, base+ADC1785_Status1_Offset, &regStatus1);
    pvme->rw(image, base+ADC1785_Status2_Offset, &regStatus2);
    regBitSet1 = pvme->swap16(regBitSet1);
    regBitSet2 = pvme->swap16(regBitSet2);
    regCtrl1 = pvme->swap16(regCtrl1);
    regStatus1 = pvme->swap16(regStatus1);
    regStatus2 = pvme->swap16(regStatus2);

    debugMsg << name << "# " << hex << "BitSet1: " << regBitSet1 << ", BitSet2: " << regBitSet2 << ", Ctrl1: " << regCtrl1 << ", Status1: " << regStatus1 << ", Status2: " << regStatus2;
    stMsg->stateOut(debugMsg);

    // mode 
    if((confValue[mode]&0x0003)==0) {
        // test ACQ mode 
        if((confValue[testACQ]&0x0001)==1) {
            debugMsg << name << "# " << "test ACQ mode";
            stMsg->stateOut(debugMsg);
            // set offline
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0002));
            // clear data
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
            pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));
            // set BitSet2 testACQ and reset
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0040));
            pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0040)); 
            // write 16 words to test event write register
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0000));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0040));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0000));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0004));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0010));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0050));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0001));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0005));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0020));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0060));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0002));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0006));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0030));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0070));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0003));
            pvme->ww(image, base+ADC1785_TestEventW_Offset, pvme->swap16((uint16_t)0x0007));
            // set BitSet2 testACQ
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0040));
        }
        // normal mode
        if((confValue[testACQ]&0x0001)==0) {
            debugMsg << name << "# " << "normal mode";
            stMsg->stateOut(debugMsg);
            // set offline
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0002));
            // clear data
            pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
            pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));
        }
    }
    // MSCT/CBLT mode
    if((confValue[mode]&0x0003)==2) {
        debugMsg << name << "# " << "MSCT/CBLT mode";
        stMsg->stateOut(debugMsg);
        // set offline
        pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0002));
        // clear data
        pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
        pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));
        // CBLT setting
        pvme->ww(image, base+ADC1785_GeoAddr_Offset, pvme->swap16(regValue[GeoAddr]&ADC1785_GeoAddr_Mask));
        pvme->ww(image, base+ADC1785_CBLTAddr_Offset, pvme->swap16(regValue[CBLTAddr]&ADC1785_CBLTAddr_Mask));
        pvme->ww(image, base+ADC1785_CBLTCtrl_Offset, pvme->swap16(regValue[CBLTCtrl]&ADC1785_CBLTCtrl_Mask));
        // reset the module to activate CBLT setting change
        pvme->ww(image, base+ADC1785_BitSet1_Offset, pvme->swap16((uint16_t)0x0080));
        pvme->ww(image, base+ADC1785_BitClear1_Offset, pvme->swap16((uint16_t)0x0080));
    }
    // memeory test mode
    if((confValue[mode]&0x0003)==1) {
        debugMsg << name << "# " << "mem test mode";
        stMsg->stateOut(debugMsg);
        // clear data
        pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
        pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));
        // test
        pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0001));
        pvme->ww(image, base+ADC1785_WTestAddr_Offset, pvme->swap16(regValue[WTestAddr]&ADC1785_WTestAddr_Mask));
        pvme->ww(image, base+ADC1785_MemTestHigh_Offset, pvme->swap16(regValue[MemTestHigh]&ADC1785_MemTestHigh_Mask));
        pvme->ww(image, base+ADC1785_MemTestLow_Offset, pvme->swap16(regValue[MemTestLow]&ADC1785_MemTestLow_Mask));
        pvme->ww(image, base+ADC1785_RTestAddr_Offset, pvme->swap16(regValue[RTestAddr]&ADC1785_RTestAddr_Mask));
    }

    return 1;
}

int mqdc32::releaseAdc() {
    pvme->freeIrq(image, confValue[irqLevel], confValue[irqVector]);
    pvme->releaseImage(image);
    return 1;
}

int mqdc32::prepAdc() {
    // D16 to D32
    uint32_t lsi0_ctl = pvme->readUniReg(0x100);
    lsi0_ctl &= 0xFF3FFFFF;

    lsi0_ctl |= D32;
    pvme->writeUniReg(0x100, lsi0_ctl);
    return 1;
}

int mqdc32::finishAdc() {
    // D32 to D16
    return 1;
}

int mqdc32::startAdc() {
    // clear data
    pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));

    // enable adc
    enableAdc();

    return 1;
}

int mqdc32::enableAdc() {
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before enable adc: " << hex << regBitSet2;
    stMsg->stateOut(debugMsg);

    regBitSet2 &= 0x0002;
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(regBitSet2));

    return 1;
}

int mqdc32::stopAdc() {
    debugMsg << name << "# " << "flush " << confValue[eventTh] << " events in the buffer.";
    stMsg->stateOut(debugMsg); 
    for(int i=0; i<confValue[eventTh]; i++) {
        pvme->ww(image, base+ADC1785_SWComm_Offset, pvme->swap16(0x0001));
    }
    
    // disable adc
    disableAdc();

    return 1;
}

int mqdc32::disableAdc() {
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before disable adc: " << hex << regBitSet2;

    stMsg->stateOut(debugMsg);
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(0x0002));

    return 1;
}

int mqdc32::accessRegNormal(const regAddrType addr, const int rw, regType* data) {
    int res;
    if(rw == 0)
        res = pvme->rw(image, base+addr, data);
    if(rw == 1)
        res = pvme->ww(image, base+addr, data);
    return res;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new mqdc32(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
