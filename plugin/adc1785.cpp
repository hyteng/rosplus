#include "adc1785.h"
#include <algorithm>
#include <string.h>

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


#define ADC1785_BASE               0xCC000000
#define ADC1785_LENGTH             0x10000
#define ADC1785_Firmware_Offset    0x1000  //R
#define ADC1785_GeoAddr_Offset     0x1002  //RW
#define ADC1785_CBLTAddr_Offset    0x1004  //RW
#define ADC1785_BitSet1_Offset     0x1006  //RW
#define ADC1785_BitClear1_Offset   0x1008  //RW
#define ADC1785_IrqLevel_Offset    0x100A  //RW
#define ADC1785_IrqVector_Offset   0x100C  //RW
#define ADC1785_Status1_Offset     0x100E  //R
#define ADC1785_Ctrl1_Offset       0x1010  //RW
#define ADC1785_AddrHigh_Offset    0x1012  //RW
#define ADC1785_AddrLow_Offset     0x1014  //RW
#define ADC1785_SSReset_Offset     0x1016  //W
#define ADC1785_CBLTCtrl_Offset    0x101A  //RW
#define ADC1785_EventTrig_Offset   0x1020  //RW
#define ADC1785_Status2_Offset     0x1022  //R
#define ADC1785_CountLow_Offset    0x1024  //R
#define ADC1785_CountHigh_Offset   0x1026  //R
#define ADC1785_IncEvent_Offset    0x1028  //W
#define ADC1785_IncOffset_Offset   0x102A  //W
#define ADC1785_LoadTest_Offset    0x102C  //RW
#define ADC1785_FCLRWindow_Offset  0x102E  //RW
#define ADC1785_BitSet2_Offset     0x1032  //RW
#define ADC1785_BitClear2_Offset   0x1034  //W(R?)
#define ADC1785_WTestAddr_Offset   0x1036  //W
#define ADC1785_MemTestHigh_Offset 0x1038  //W
#define ADC1785_MemTestLow_Offset  0x103A  //W
#define ADC1785_CrateSelect_Offset 0x103C  //RW
#define ADC1785_TestEventW_Offset  0x103E  //W
#define ADC1785_CountReset_Offset  0x1040  //W
#define ADC1785_RTestAddr_Offset   0x1064  //W
#define ADC1785_SWComm_Offset      0x1068  //W
#define ADC1785_SlideConst_Offset  0x106A  //RW
#define ADC1785_AAD_Offset         0x1070  //R
#define ADC1785_BAD_Offset         0x1072  //R
#define ADC1785_ThCh0H_Offset      0x1080  //RW
#define ADC1785_ThCh0L_Offset      0x1084  //RW
#define ADC1785_ThCh1H_Offset      0x1088  //RW
#define ADC1785_ThCh1L_Offset      0x108C  //RW
#define ADC1785_ThCh2H_Offset      0x1090  //RW
#define ADC1785_ThCh2L_Offset      0x1094  //RW
#define ADC1785_ThCh3H_Offset      0x1098  //RW
#define ADC1785_ThCh3L_Offset      0x109C  //RW
#define ADC1785_ThCh4H_Offset      0x10A0  //RW
#define ADC1785_ThCh4L_Offset      0x10A4  //RW
#define ADC1785_ThCh5H_Offset      0x10A8  //RW
#define ADC1785_ThCh5L_Offset      0x10AC  //RW
#define ADC1785_ThCh6H_Offset      0x10B0  //RW
#define ADC1785_ThCh6L_Offset      0x10B4  //RW
#define ADC1785_ThCh7H_Offset      0x10B8  //RW
#define ADC1785_ThCh7L_Offset      0x10BC  //RW

#define ADC1785_Firmware_Mask    0xFFFF  
#define ADC1785_GeoAddr_Mask     0x001F  
#define ADC1785_CBLTAddr_Mask    0x00FF  
#define ADC1785_BitSet1_Mask     0x0098  
#define ADC1785_BitClear1_Mask   0x0098  
#define ADC1785_IrqLevel_Mask    0x0007  
#define ADC1785_IrqVector_Mask   0x00FF  
#define ADC1785_Status1_Mask     0x01FF  
#define ADC1785_Ctrl1_Mask       0x0074  
#define ADC1785_AddrHigh_Mask    0x00FF  
#define ADC1785_AddrLow_Mask     0x00FF  
#define ADC1785_SSReset_Mask     0xFFFF  
#define ADC1785_CBLTCtrl_Mask    0x0003  
#define ADC1785_EventTrig_Mask   0x001F  
#define ADC1785_Status2_Mask     0x00F6  
#define ADC1785_CountLow_Mask    0xFFFF  
#define ADC1785_CountHigh_Mask   0x00FF  
#define ADC1785_IncEvent_Mask    0xFFFF  
#define ADC1785_IncMask_Mask     0xFFFF  
#define ADC1785_LoadTest_Mask    0xFFFF  
#define ADC1785_FCLRWindow_Mask  0x03FF  
#define ADC1785_BitSet2_Mask     0x7FFF  
#define ADC1785_BitClear2_Mask   0x7FFF  
#define ADC1785_WTestAddr_Mask   0x07FF  
#define ADC1785_MemTestHigh_Mask 0xFFFF  
#define ADC1785_MemTestLow_Mask  0xFFFF  
#define ADC1785_CrateSelect_Mask 0x00FF  
#define ADC1785_TestEventW_Mask  0x1FFF  
#define ADC1785_CountReset_Mask  0xFFFF  
#define ADC1785_RTestAddr_Mask   0x07FF  
#define ADC1785_SWComm_Mask      0xFFFF  
#define ADC1785_SlideConst_Mask  0x00FF  
#define ADC1785_AAD_Mask         0x0FFF  
#define ADC1785_BAD_Mask         0x0FFF  
#define ADC1785_ThCh0H_Mask      0x01FF  
#define ADC1785_ThCh0L_Mask      0x01FF  
#define ADC1785_ThCh1H_Mask      0x01FF  
#define ADC1785_ThCh1L_Mask      0x01FF  
#define ADC1785_ThCh2H_Mask      0x01FF  
#define ADC1785_ThCh2L_Mask      0x01FF  
#define ADC1785_ThCh3H_Mask      0x01FF  
#define ADC1785_ThCh3L_Mask      0x01FF  
#define ADC1785_ThCh4H_Mask      0x01FF  
#define ADC1785_ThCh4L_Mask      0x01FF  
#define ADC1785_ThCh5H_Mask      0x01FF  
#define ADC1785_ThCh5L_Mask      0x01FF  
#define ADC1785_ThCh6H_Mask      0x01FF  
#define ADC1785_ThCh6L_Mask      0x01FF  
#define ADC1785_ThCh7H_Mask      0x01FF  
#define ADC1785_ThCh7L_Mask      0x01FF  

#define EVENTSIZE 72

// status and contrl
enum ADC1785Conf {geoAddr=0, cbltAddr, irqLevel, irqVector, addrHigh, addrLow, eventTh, loadTest, fclrw, crateSel, slideConst, threshold0H, threshold0L, threshold1H, threshold1L, threshold2H, threshold2L, threshold3H, threshold3L, threshold4H, threshold4L, threshold5H, threshold5L, threshold6H, threshold6L, threshold7H, threshold7L, /*WO*/ssReset, cbltCtrl, incEvent, incOffset, wTestAddr, memTestHigh, memTestLow, testEventW, countReset, rTestAddr, swComm, /*BitSet*/berrFlag, selAddr, softReset, blkend, progResetMod, busError, align64, mode, offline, clearData, overRange, lowTh, testACQ, slideScale, stepTh, autoInc, emptyProg, slideSub, allTrigger, /*RO*/firmware, DRDY, gDRDY, busy, gbusy, purged, termOn, termOff, EVRDY, buffEmpty, buffFull, DSel0, DSel1, CSel0, CSel1, eventCountLow, eventCountHigh, aad, bad, /*Size*/ConfSize};
#define ConfRO 19

enum ADC1785Reg {/*RW*/GeoAddr=0, CBLTAddr, IrqLevel, IrqVector, AddrHigh, AddrLow, EventTrig, LoadTest, FCLRWindow, CrateSelect, SlideConst, ADC1785_ThCh0H, ADC1785_ThCh0L, ADC1785_ThCh1H, ADC1785_ThCh1L, ADC1785_ThCh2H, ADC1785_ThCh2L, ADC1785_ThCh3H, ADC1785_ThCh3L, ADC1785_ThCh4H, ADC1785_ThCh4L, ADC1785_ThCh5H, ADC1785_ThCh5L, ADC1785_ThCh6H, ADC1785_ThCh6L, ADC1785_ThCh7H, ADC1785_ThCh7L, /*WO*/SSReset, CBLTCtrl, IncEvent, IncOffset, WTestAddr, MemTestHigh, MemTestLow, TestEventW, CountReset, RTestAddr, SWComm, /*BitSet*/BitSet1, BitClear1, Ctrl1, BitSet2, BitClear2, /*RO*/Firmware, Status1, Status2, CountLow, CountHigh, AAD, BAD, /*Size*/RegSize};
#define RegRW 27
#define RegWO 11
#define RegBitSet 5
#define RegRO 7


// ctrl, conf, and reg infomation

#define ctrlSize 73
#define ctrlReg 36
#define ctrlBitSet1 3
#define ctrlCtrl1 4
#define ctrlBitSet2 12
#define ctrlRO 18
#define ctrlStatus1 8
#define ctrlStatus2 6

string ctrl1785[ctrlSize] = {"geoAddr", "cbltAddr", "irqLevel", "irqVector", "vmeAddr", "eventTh", "loadTest", "fclrw", "crateSel", "slideConst", "threshold0H", "threshold0L", "threshold1H", "threshold1L", "threshold2H", "threshold2L", "threshold3H", "threshold3L", "threshold4H", "threshold4L", "threshold5H", "threshold5L", "threshold6H", "threshold6L", "threshold7H", "threshold7L", /*WO*/"ssReset", "cbltCtrl", "incEvent", "incOffset", "wTestAddr", "memTestW", "testEventW", "countReset", "rTestAddr", "swComm", /*BitSet1*/"berrFlag", "selAddr", "softReset", /*Ctrl1*/"blkend", "progResetMod", "busError", "align64", /*BitSet2*/"mode", "offline", "clearData", "overRange", "lowTh", "testACQ", "slideScale", "stepTh", "autoInc", "emptyProg", "slideSub", "allTrigger", /*RO*/"firmware", /*Status1*/"DRDY", "gDRDY", "busy", "gbusy", "purged", "termOn", "termOff", "EVRDY", /*Status2*/"buffEmpty", "buffFull", "DSel0", "DSel1", "CSel0", "CSel1", "eventCount", "aad", "bad"};

unsigned int ctrlDefault[]={0,0xAA,7,0,0x00CC0000,8,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,8,0,4,0,0,0,/*BitSet1*/0,0,0,/*Ctrl1*/0,0,0,0,/*BitSet2*/0,0,0,1,1,0,1,1,1,0,0,1};

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

string confName1785[confSize] = {"geoAddr", "cbltAddr", "irqLevel", "irqVector", "addrHigh", "addrLow", "eventTh", "loadTest", "fclrw", "crateSel", "slideConst", "threshold0H", "threshold0L", "threshold1H", "threshold1L", "threshold2H", "threshold2L", "threshold3H", "threshold3L", "threshold4H", "threshold4L", "threshold5H", "threshold5L", "threshold6H", "threshold6L", "threshold7H", "threshold7L", "ssReset", "cbltCtrl", "incEvent", "incOffset", "wTestAddr", "memTestHigh", "memTestLow", "testEventW", "countReset", "rTestAddr", "swComm", /*BitSet1*/"berrFlag", "selAddr", "softReset", /*BitClear1*/"berrFlag~", "selAddr~", "softReset~", /*Ctrl1*/"blkend", "progResetMod", "busError", "align64", /*BitSet2*/"mode", "offline", "clearData", "overRange", "lowTh", "testACQ", "slideScale", "stepTh", "autoInc", "emptyProg", "slideSub", "allTrigger", /*BitClear2*/"mode~", "offline~", "clearData~", "overRange~", "lowTh~", "testACQ~", "slideScale~", "stepTh~", "autoInc~", "emptyProg~", "slideSub~", "allTrigger~", /*RO*/"firmware", /*Status1*/"DRDY", "gDRDY", "busy", "gbusy", "purged", "termOn", "termOff", "EVRDY", /*Status2*/"buffEmpty", "buffFull", "DSel0", "DSel1", "CSel0", "CSel1", "eventCountLow", "eventCountHigh", "aad", "bad"};

uint16_t confMask1785[confSize] = {0x001F, 0x00FF, 0x0007, 0x00FF, 0x00FF, 0x00FF, 0x001F, 0xFFFF, 0x03FF, 0x00FF, 0x00FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0x01FF, 0xFFFF, 0x0003, 0xFFFF, 0xFFFF, 0x07FF, 0xFFFF, 0xFFFF, 0x1FFF, 0xFFFF, 0x07FF, 0xFFFF, /*BitSet1*/0x0008, 0x0010, 0x0080, /*BitClear1*/0x0008, 0x0010, 0x0080, /*Ctrl1*/0x0004, 0x0010, 0x0020, 0x0040, /*BitSet2*/0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0040, 0x0080, 0x0100, 0x0800, 0x1000, 0x2000, 0x4000, /*BitClear2*/0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0040, 0x0080, 0x0100, 0x0800, 0x1000, 0x2000, 0x4000, /*RO*/0xFFFF, /*Status1*/0x0001, 0x0002, 0x0004, 0x0008, 0x0020, 0x0040, 0x0080, 0x0100, /*Status2*/0x0002, 0x0004, 0x0010, 0x0020, 0x0040, 0x0080, 0xFFFF, 0x00FF, 0x0FFF, 0x0FFF};

unsigned int confDefault1785[]={0,0xAA,7,0,0x00CC,0x0000,8,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,8,0,0,4,0,0,0,/*BitSet1*/0,0,0,/*Ctrl1*/0,0,0,0,/*BitSet2*/0,0,0,1,1,0,1,1,1,0,0,1};

#define regSize 50
#define regRW 27
#define regWO 11
#define regBitSet 5
#define regRO 7

uint32_t regAddr1785[regSize] = {0x1002/*geoAddr*/, 0x1004/*cbltAddr*/, 0x100A/*irqLevel*/, 0x100C/*irqVector*/, 0x1012/*addrHigh*/, 0x1014/*addrLow*/, 0x1020/*eventTh*/, 0x102C/*loadTest*/, 0x102E/*fclrw*/, 0x103C/*crateSel*/, 0x106A/*slideConst*/, 0x1080/*threshold0H*/, 0x1084/*threshold0L*/, 0x1088/*threshold1H*/, 0x108C/*threshold1L*/, 0x1090/*threshold2H*/, 0x1094/*threshold2L*/, 0x1098/*threshold3H*/, 0x109C/*threshold3L*/, 0x10A0/*threshold4H*/, 0x10A4/*threshold4L*/, 0x10A8/*threshold5H*/, 0x10AC/*threshold5L*/, 0x10B0/*threshold6H*/, 0x10B4/*threshold6L*/, 0x10B8/*threshold7H*/, 0x10BC/*threshold7L*/, 0x1016/*ssReset*/, 0x101A/*cbltCtrl*/, 0x1028/*incEvent*/, 0x102A/*incOffset*/, 0x1036/*wTestAddr*/, 0x1038/*memTestHigh*/, 0x103A/*memTestLow*/, 0x103E/*testEventW*/, 0x1040/*countReset*/, 0x1064/*rTestAddr*/, 0x1068/*swComm*/, 0x1006/*bitSet1*/, 0x1008/*bitClear1*/, 0x1010/*ctrl1*/, 0x1032/*bitSet2*/, 0x1034/*bitClear2*/, 0x1000/*firmware*/, 0x100E/*status1*/, 0x1022/*status2*/, 0x1024/*countLow*/, 0x1026/*countHigh*/, 0x1070/*aad*/, 0x1072/*bad*/};


static map<string, vector<string> > adc1785_ctrl2conf;
static map<string, int> adc1785_conf2reg;
static map<string, uintptr_t>adc1785_conf2mask;
static vector<uintptr_t> adc1785_regAddr;
static vector<int> adc1785_regRWIdx;

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
    adc1785_regRWIdx.clear();
    for(int i=0; i<regSize; i++) {
        adc1785_regAddr.push_back((uintptr_t)(&regAddr1785[i]));
        int rw = 0;
        if(i>=regRW && i<regRW+regWO)
            rw = 2;
        if(i>=regRW+regWO+regBitSet)
            rw = 1;
        adc1785_regRWIdx.push_back(rw);
    }

    return 1;
}

static int dummy = setCtrl();

#define wordSize 4

adc1785::adc1785(const string& n): smBase(n) {
    ctrl2conf = &adc1785_ctrl2conf;
    conf2reg = &adc1785_conf2reg;
    conf2mask = &adc1785_conf2mask;
    regAddr = &adc1785_regAddr;
    regRWIdx = &adc1785_regRWIdx;

    vd = new regUint16();
    vm = new regUint16();
}

adc1785::~adc1785() {
}

int adc1785::queryInterface(const string& funcName, void* para[], void* ret) {
    if(funcName == "run") {
        run();
        return 1;
    }
    if(funcName == "getBuffAddr") {
        *(uintptr_t*)ret = getBuffAddr();
        return 1;
    }
    if(funcName == "getEventTh") {
        *(uint32_t*)ret = getEventTh();
        return 1;
    }
    if(funcName == "getTranSize") {
        *(uint32_t*)ret = getTranSize();
        return 1;
    }
    if(funcName == "packData") {
        return packDataTest(*(unsigned int*)para[0]);
    }
    if(funcName == "fillEvent") {
        return fillEvent(*(unsigned int*)para[0]);
    }
    return 0;
}

int adc1785::InitializedLOAD(void* argv[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return 2;
}

int adc1785::LoadedUNLD(void* argv[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return 1;
}

int adc1785::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //if(!configAdc())
        //return -1;
    // for test
    confValue[eventTh] = 2;
    return 3;
}

int adc1785::ConfiguredUNCF(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseAdc();
    return 2;
}

int adc1785::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepAdc();
    return 4;
}

int adc1785::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    finishAdc();
    return 3;
}

int adc1785::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    //startAdc();
    return 5;
}

int adc1785::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int adc1785::RunningPAUS(void* argv[]) {
    debugMsg << name << "# " << "RunningPAUS";
    stMsg->stateOut(debugMsg);
    //disableAdc();
    return 6;
}

int adc1785::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int adc1785::PausedRESU(void* argv[]) {
    debugMsg << name << "# " << "PausedRESU";
    stMsg->stateOut(debugMsg);
    //enableAdc();
    return 5;
}

int adc1785::OTFCTRL(void* argv[]) {
    // D32 to D16
    //finishAdc();
    // call smBase::OTFCTRL
    smBase::OTFCTRL(argv);
    // D16 to D32
    //prepAdc();

    return (int)stId;
}

int adc1785::accessReg(const int idx, const int rw, regData& data) {
    int res;
    if(idx<0 || idx>=regSize || rw<0 || rw>1 || data.getValueP()==NULL )
        return 0;

    if((*regRWIdx)[idx] != 0 && (*regRWIdx)[idx] != rw+1)
        return 0;

    regAddrType addr = *(regAddrType*)((*regAddr)[idx]);
    
    res = accessRegNormal(addr, rw, (regType*)data.getValueP());
    return res;
}

int adc1785::maskRegData(regData& data, regData& mask) {
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

int adc1785::unmaskRegData(regData& data, regData& mask) {
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

int adc1785::run() {
    //pvme->waitIrq(confValue[irqLevel], confValue[irqVector]);
    debugMsg << name << "# " << "triggered";
    stMsg->stateOut(debugMsg);
    return 1;
}

uintptr_t adc1785::getBuffAddr() {
    return base;
}

uint32_t adc1785::getEventTh() {
    return confValue[eventTh];
}

uint32_t adc1785::getTranSize() {
    return confValue[eventTh];
}

int adc1785::packData(unsigned int &packSize) {
    unsigned int tmpIdx;
    dataPool->devSetSnap();
    unsigned int bias = 0;
    void* p;
    unsigned int value;
    unsigned int tranSize = 0;
    unsigned int readSize, restSize, j;
    for(unsigned int i=0; i<packSize/wordSize; i++,bias+=wordSize) {
        readSize = wordSize;
        p = dataPool->devGetSnapPtr(bias, readSize);
        if(p == NULL)
            return 0;
        if(readSize < wordSize) {
            for(j=0; j<readSize; j++)
                v[j] = *((char*)p+j);
            restSize = wordSize-readSize;
            p = dataPool->devGetSnapPtr(bias+readSize, restSize);
            for(j=readSize; j<wordSize; j++)
                v[j] = *((char*)p+j);
            value = (unsigned int)(*(uint32_t*)v);
        }
        else {
            value = (unsigned int)*(uint32_t*)p;
        }

        // invalid data
        if((value&0x00000007) == 0x00000006) {
            tmpIdx=0;
            continue;
        }
        // header
        if((value&0x00000007) == 0x00000002) {
            //memset(tmp, 0, EVENTSIZE);
            if(eventPtrW == -1)
                return 0;
            eventSet[eventPtrW][0] = value;
            tmpIdx=1;
            continue;
        }
        // valid data
        if((value&0x00000007) == 0x00000000 && tmpIdx > 0 && tmpIdx < 17) {
            eventSet[eventPtrW][tmpIdx]=value;
            tmpIdx++;
            continue;
        }
        // end of event
        if((value&0x00000007) == 0x00000004 && tmpIdx > 0 && tmpIdx < 18) {
            eventSet[eventPtrW][tmpIdx] = value;
            tmpIdx++;
            // copy to idx set
            eventIdx->push(tmpIdx);
            //dataPool->netWrite(tmp, tmpIdx*4);
            tranSize += tmpIdx*4;
            debugMsg << name << "# " << "pack data: " << endl;
            for(int i=0; i<tmpIdx; i++) {
                debugMsg << std::hex << eventSet[eventPtrW][i] << " ";
            }
            stMsg->stateOut(debugMsg);
            tmpIdx=0;
            if(eventPtrR == -1)
                eventPtrR = eventPtrW;
            eventPtrW++;
            if(eventPtrW==confValue[eventTh]*2)
                eventPtrW = 0;
            if(eventPtrW == eventPtrR) {
                debugMsg << name << "# " << "event pack buffer full";
                stMsg->stateOut(debugMsg);
                eventPtrW = -1;
            }
            continue;
        }
        // reserved data
    }

    unsigned int popSize = dataPool->devPopSnap(packSize);
    if(popSize != packSize)
        return 0;

    packSize = tranSize;
    return 1;
}


int adc1785::packDataTest(unsigned int& packSize) {
    unsigned int tmpIdx = 0;
    dataPool->devSetSnap();
    unsigned int bias = 0;
    void* p;
    unsigned int value;
    unsigned int tranSize = 0;
    unsigned int readSize, restSize, j;
    for(unsigned int i=0; i<packSize/wordSize; i++,bias+=wordSize) {
        debugMsg << name << "# " << "pack data " << i;
        stMsg->stateOut(debugMsg);
        readSize = wordSize;
        p = dataPool->devGetSnapPtr(bias, readSize);
        if(p == NULL)
            return 0;
        if(readSize < wordSize) {
            for(j=0; j<readSize; j++)
                v[j] = *((char*)p+j);
            restSize = wordSize-readSize;
            p = dataPool->devGetSnapPtr(bias+readSize, restSize);
            for(j=readSize; j<wordSize; j++)
                v[j] = *((char*)p+j);
            value = (unsigned int)(*(uint32_t*)v);
        }
        else {
            value = (unsigned int)*(uint32_t*)p;
        }

        if(eventPtrW == -1)
            return 0;
        eventSet[eventPtrW][tmpIdx] = value;
        debugMsg << name << "# " << "pack data value " << eventSet[eventPtrW][tmpIdx];
        stMsg->stateOut(debugMsg);
        tmpIdx++;
        if(tmpIdx == 2) {
            debugMsg << name << "# " << "pack event";
            stMsg->stateOut(debugMsg);
            // copy to idx set
            eventIdx->push(tmpIdx);
            //dataPool->netWrite(tmp, tmpIdx*4);
            tranSize += tmpIdx*4;
            debugMsg << name << "# " << "pack data: " << endl;
            for(int i=0; i<tmpIdx; i++) {
                debugMsg << std::hex << eventSet[eventPtrW][i] << " ";
            }
            stMsg->stateOut(debugMsg);
            tmpIdx=0;
            if(eventPtrR == -1)
                eventPtrR = eventPtrW;
            eventPtrW++;
            if(eventPtrW==confValue[eventTh]*2)
                eventPtrW = 0;
            if(eventPtrW == eventPtrR) {
                debugMsg << name << "# " << "event pack buffer full";
                stMsg->stateOut(debugMsg);
                eventPtrW = -1;
            }
            continue;
        }
    }
    unsigned int popSize = dataPool->devPopSnap(packSize);
    if(popSize != packSize)
        return 0;

    packSize = tranSize;
    return 1;
}

int adc1785::fillEvent(unsigned int& packSize) {
    if(eventPtrR==-1 || eventIdx->size()==0) {
        packSize = 0;
        return 0;
    }
    packSize = eventIdx->front() * wordSize;
    dataPool->netWrite(&eventSet[eventPtrR][0], packSize);
    if(sendData == true)
        stMsg->sendData(name+"#", &eventSet[eventPtrR][0], packSize);
    eventIdx->pop();
    if(eventPtrW == -1)
        eventPtrW = eventPtrR;
    eventPtrR++;
    if(eventPtrR == confValue[eventTh]*2)
        eventPtrR = 0;
    if(eventPtrW == eventPtrR) {
        debugMsg << name << "# " << "event pack buffer empty";
        stMsg->stateOut(debugMsg);
        eventPtrR = -1;
    }
    return 1;
}

int adc1785::configAdc() {
    int res;

    pvme = NULL;
    string vmeModeName;
    if((res=cfgInfo->infoGetString("config."+name+".vmeModeName", vmeModeName)) == 1) {
        std::vector< std::pair<std::string, smBase*> >::const_iterator iter;
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == vmeModeName)
                if(!iter->second->queryInterface("getVME", NULL, (void*)&pvme))
                    return 0;
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

int adc1785::releaseAdc() {
    pvme->freeIrq(image, confValue[irqLevel], confValue[irqVector]);
    pvme->releaseImage(image);
    return 1;
}

int adc1785::prepAdc() {
    if(eventSet != NULL)
        delete [] eventSet;
    eventSet = new uint32_t[confValue[eventTh]*2][ADC1785EVENTUINTSIZE];
    eventPtrW = 0;
    eventPtrR = -1;
    if(eventIdx != NULL)
        delete eventIdx;
    eventIdx = new std::queue<unsigned int>;
    return 1;
}

int adc1785::finishAdc() {
    if(eventSet != NULL)
        delete [] eventSet;
    eventPtrW = 0;
    eventPtrR = -1;
    if(eventIdx != NULL)
        delete eventIdx;
    return 1;
}

int adc1785::startAdc() {
    // clear data
    pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));

    // enable adc
    enableAdc();

    return 1;
}

int adc1785::stopAdc() {
    debugMsg << name << "# " << "flush " << confValue[eventTh] << " events in the buffer.";
    stMsg->stateOut(debugMsg); 
    for(int i=0; i<confValue[eventTh]; i++) {
        pvme->ww(image, base+ADC1785_SWComm_Offset, pvme->swap16(0x0001));
    }
    
    // disable adc
    disableAdc();

    return 1;
}

int adc1785::enableAdc() {
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before enable adc: " << hex << regBitSet2;
    stMsg->stateOut(debugMsg);

    regBitSet2 &= 0x0002;
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(regBitSet2));

    return 1;
}

int adc1785::disableAdc() {
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before disable adc: " << hex << regBitSet2;
    stMsg->stateOut(debugMsg);
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(0x0002));

    return 1;
}

int adc1785::accessRegNormal(const regAddrType addr, const int rw, regType* data) {
    int res;
    if(rw == 0)
        res = pvme->rw(image, base+addr, data);
    if(rw == 1)
        res = pvme->ww(image, base+addr, data);
    return res;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new adc1785(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}


