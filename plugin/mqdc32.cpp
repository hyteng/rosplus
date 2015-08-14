#include "mqdc32.h"

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
#define MQDC32_CBLTControl_Offset  	0x6020
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
#define MQDC32_BusyTimeLow_Offset  	0x60A0 //RO
#define MQDC32_BusyTimeHigh_Offset 	0x60A2 //RO
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
#define MQDC32_CBLTControl_Mask		0x00FF
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
#define MQDC32_PulserStatus_Mask	0x0007
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
#define MQDC32_BusyTimeLow_Mask		0xFFFF //RO
#define MQDC32_BusyTimeHigh_Mask	0xFFFF //RO
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

#define ctrlSize 107
#define ctrlCtrl 4
#define ctrlRO 18

string ctrlMQDC32[ctrlSize] = {"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", "addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", "irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", "bltCtrl", "cbltAddr", "mcstAddr", "dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", "bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", "limitBank0", "limitBank1", "expDelay0", "expDelay1", "inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", "pulserSt", "pulserDac", "rcBusNo", "rcModNum", "rcOpCode", "rcSt", "ctraReset", "ctrbReset", "extReset", "eventCounter", "tsSource", "tsDivisor", "tsCounter", "busyTime", "gate1Time", "time0", "tim1", "time2", "stopCounterA", "stopCounterB", "limit0High", "limit0Low", "limit1High", "limit1Low"};

int ctrlRWMQDC32[ctrlSize] = {/*ChTh*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*board*/0, 0, 0, 0, 2, 1, /*irq*/0, 0, 2, 2, 0, 0, 0, /*blt*/0, 0, 1, /*fifo*/1, 0, 2, 0, 0, 0, 0, 0, 2, 1, /*adc*/0, 0, 0, 0, 0, 0, 0, 0, /*trigger*/0, 0, 0, 0, /**/};

unsigned int ctrlDefault[]={};

#define confSize 91
#define confReg 38
#define confBitSet1 3
#define confBitClear1 3
#define confCtrl1 4
#define confBitSet2 12
#define confBitClear2 12
#define confRO 19
#define confStatus1 8
#define confStatus2 6

string confNameMQDC32[confSize] = {"Ch00Th", "Ch01Th", "Ch02Th", "Ch03Th", "Ch04Th", "Ch05Th", "Ch06Th", "Ch07Th", "Ch08Th", "Ch09Th", "Ch10Th", "Ch11Th", "Ch12Th", "Ch13Th", "Ch14Th", "Ch15Th", "Ch16Th", "Ch17Th", "Ch18Th", "Ch19Th", "Ch20Th", "Ch21Th", "Ch22Th", "Ch23Th", "Ch24Th", "Ch25Th", "Ch26Th", "Ch27Th", "Ch28Th", "Ch29Th", "Ch30Th", "Ch31Th", "addrSource", "addrReg", "moduleId", "fastVme", "softReset", "firmware", "irqLevel", "irqVector", "irqTest", "irqReset", "irqTh", "maxTransfer", "irqWithdraw", "bltCtrl", "cbltLast", "cbltFirst", "mcstCtrl", "cbltAddr", "mcstAddr", "dataLength", "dataFormat", "readoutReset", "multiEvent", "skipBerr", "countUnit", "markType", "startAcq", "fifoReset", "dataReady", "bankConnect", "switchPull0", "switchPull1", "shiftBank0", "shiftBank1", "slideScale", "overFlow", "disableTh", "limitBank0", "limitBank1", "expDelay0", "expDelay1", "inputCouple0", "inputCouple1", "BLR", "terGate0", "terGate1", "terReset", "terBank0", "terBank1", "eclGate1Osc", "eclFCReset", "gateSelect", "nimGate1Osc", "nimFCReset", "nimBusy", "pulserSt", "pulserDac", "rcBusNo", "rcModNum", "rcOpCode", "rcSt", "ctraReset", "ctrbReset", "extReset", "eventCounterLow", "eventCounterHigh", "tsSource", "tsDivisor", "tsCounterLow", "tsCounterHigh", "busyTimeLow", "busyTimeHigh", "gate1TimeLow", "gate1TimeHigh", "time0", "tim1", "time2", "stopCounterA", "stopCounterB", "limit0High", "limit0Low", "limit1High", "limit1Low"};

uint16_t confMaskMQDC32[confSize] = {};

unsigned int confDefault[]={};

#define regSize 50
#define regRW 27
#define regWO 11
#define regBitSet 5
#define regRO 7

uint32_t regAddrMQDC32[regSize] = {0x0000/*fifo*/, 0x1004/*cbltAddr*/, 0x100A/*irqLevel*/, 0x100C/*irqVector*/, 0x1012/*addrHigh*/, 0x1014/*addrLow*/, 0x1020/*eventTh*/, 0x102C/*loadTest*/, 0x102E/*fclrw*/, 0x103C/*crateSel*/, 0x106A/*slideConst*/, 0x1080/*threshold0H*/, 0x1084/*threshold0L*/, 0x1088/*threshold1H*/, 0x108C/*threshold1L*/, 0x1090/*threshold2H*/, 0x1094/*threshold2L*/, 0x1098/*threshold3H*/, 0x109C/*threshold3L*/, 0x10A0/*threshold4H*/, 0x10A4/*threshold4L*/, 0x10A8/*threshold5H*/, 0x10AC/*threshold5L*/, 0x10B0/*threshold6H*/, 0x10B4/*threshold6L*/, 0x10B8/*threshold7H*/, 0x10BC/*threshold7L*/, 0x1016/*ssReset*/, 0x101A/*cbltCtrl*/, 0x1028/*incEvent*/, 0x102A/*incOffset*/, 0x1036/*wTestAddr*/, 0x1038/*memTestHigh*/, 0x103A/*memTestLow*/, 0x103E/*testEventW*/, 0x1040/*countReset*/, 0x1064/*rTestAddr*/, 0x1068/*swComm*/, 0x1006/*bitSet1*/, 0x1008/*bitClear1*/, 0x1010/*ctrl1*/, 0x1032/*bitSet2*/, 0x1034/*bitClear2*/, 0x1000/*firmware*/, 0x100E/*status1*/, 0x1022/*status2*/, 0x1024/*countLow*/, 0x1026/*countHigh*/, 0x1070/*aad*/, 0x1072/*bad*/};

static map<string, vector<string> > mqdc32_ctrl2conf;
static map<string, int> mqdc32_conf2reg;
static map<string, uintptr_t>mqdc32_conf2mask;
static vector<uintptr_t> mqdc32_regAddr;
static vector<int> mqdc32_regWOIdx, mqdc32_regROIdx;

static int setCtrl() {
    adc1785_ctrl2conf.clear();
    for(int i=0,j=0; i<ctrlSize; i++, j++) {
        adc1785_ctrl2conf[ctrl1785[i]] = vector<string>(0);
        adc1785_ctrl2conf[ctrl1785[i]].push_back(confName1785[j]);
        
        if(i==4 || i==31 || i==70) {
            j++;
            adc1785_ctrl2conf[ctrl1785[i]].push_back(confName1785[j]);
        }

        if(i>=ctrlReg && i<ctrlReg+ctrlBitSet1)
            adc1785_ctrl2conf[ctrl1785[i]].push_back(confName1785[j+ctrlBitSet1]);
        if(i==ctrlReg+ctrlBitSet1-1)
            j+=ctrlBitSet1;

        if(i>=ctrlReg+ctrlBitSet1+ctrlCtrl1 && i<ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2)
            adc1785_ctrl2conf[ctrl1785[i]].push_back(confName1785[j+ctrlBitSet2]);
        if(i==ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2-1)
            j+=ctrlBitSet2;
    }

    adc1785_conf2reg.clear();
    adc1785_conf2mask.clear();
    for(int i=0, j=0; i<confSize; i++,j++) {
        if(i>=confReg && i<confReg+confBitSet1)
            j = confReg;
        if(i>=confReg+confBitSet1 && i<confReg+confBitSet1+confBitClear1)
            j = confReg+1;
        if(i>=confReg+confBitSet1+confBitClear1 && i<confReg+confBitSet1+confBitClear1+confCtrl1)
            j = confReg+2;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2)
            j = confReg+3;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2)
            j = confReg+4;
        if(i==confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2)
            j = confReg+5;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1)
            j = confReg+6;
        if(i>=confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1 && i<confReg+confBitSet1+confBitClear1+confCtrl1+confBitSet2+confBitClear2+1+confStatus1+confStatus2)
            j = confReg+7;

        adc1785_conf2reg[confName1785[i]] = j;
        adc1785_conf2mask[confName1785[i]] = (uintptr_t)(&confMask1785[i]);
    }

    adc1785_regAddr.clear();
    for(int i=0; i<regSize; i++) {
        adc1785_regAddr.push_back((uintptr_t)(&regAddr1785[i]));
    }
    adc1785_regWOIdx.clear();
    for(int i=regRW; i<regRW+regWO; i++)
        adc1785_regWOIdx.push_back(i);
    adc1785_regROIdx.clear();
    for(int i=regRW+regWO+regBitSet; i<regRW+regWO+regBitSet+regRO; i++)
        adc1785_regROIdx.push_back(i);

    return 1;
}


static int setCtrl() {
    return 1;
}

static int dummy = setDev();

mqdc32::mqdc32(const string& n): smBase(n) {
    ctrl2conf = &mqdc32_ctrl2conf;
    conf2reg = &mqdc32_conf2reg;
    conf2mask = &mqdc32_conf2mask;
    regAddr = &mqdc32_regAddr;
    regWOIdx = &mqdc32_regWOIdx;
    regROIdx = &mqdc32_regROIdx;

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
