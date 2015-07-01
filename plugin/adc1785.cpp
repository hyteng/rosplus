#include "adc1785.h"

using std::string;
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

// status and contrl
enum ADC1785Conf {geoAddr=0, cbltAddr, irqLevel, irqVector, addrHigh, addrLow, eventTh, loadTest, fclrw, crateSel, slideConst, threshold0H, threshold0L, threshold1H, threshold1L, threshold2H, threshold2L, threshold3H, threshold3L, threshold4H, threshold4L, threshold5H, threshold5L, threshold6H, threshold6L, threshold7H, threshold7L, /*WO*/ssReset, cbltBoard, incEvent, incOffset, wTestAddr, memTestHigh, memTestLow, testEventW, countReset, rTestAddr, swComm, /*BitSet*/berrFlag, selAddr, softReset, blkend, progResetMod, busError, align64, mode, offline, clearData, overRange, lowTh, testACQ, slideScale, stepTh, autoInc, emptyProg, slideSub, allTrigger, /*RO*/eventCount, buffEmpty, buffFull, CSel0, CSel1, DSel0, DSel1, DRDY, gDRDY, busy, gbusy, purged, termOn, EVRDY, /*Size*/confSize};
#define confRO 14

enum ADC1785Reg {/*RW*/GeoAddr=0, CBLTAddr, IrqLevel, IrqVector, AddrHigh, AddrLow, EventTrig, LoadTest, FCLRWindow, CrateSelect, SlideConst, ADC1785_ThCh0H, ADC1785_ThCh0L, ADC1785_ThCh1H, ADC1785_ThCh1L, ADC1785_ThCh2H, ADC1785_ThCh2L, ADC1785_ThCh3H, ADC1785_ThCh3L, ADC1785_ThCh4H, ADC1785_ThCh4L, ADC1785_ThCh5H, ADC1785_ThCh5L, ADC1785_ThCh6H, ADC1785_ThCh6L, ADC1785_ThCh7H, ADC1785_ThCh7L, /*WO*/SSReset, CBLTCtrl, IncEvent, IncOffset, WTestAddr, MemTestHigh, MemTestLow, TestEventW, CountReset, RTestAddr, SWComm, /*BitSet*/BitSet1, BitClear1, Ctrl1, BitSet2, BitClear2, /*RO*/Firmware, Status1, Status2, CountLow, CountHigh, AAD, BAD, /*Size*/regSize};
#define regRW 27
#define regWO 11
#define regBitSet 5
#define regRO 7

string confName[confSize] = {"geoAddr", "cbltAddr", "irqLevel", "irqVector", "addrHigh", "addrLow", "eventTh", "loadTest", "fclrw", "crateSel", "slideConst", "threshold0H", "threshold0L", "threshold1H", "threshold1L", "threshold2H", "threshold2L", "threshold3H", "threshold3L", "threshold4H", "threshold4L", "threshold5H", "threshold5L", "threshold6H", "threshold6L", "threshold7H", "threshold7L", "ssReset", "cbltBoard", "incEvent", "incOffset", "wTestAddr", "memTestHigh", "memTestLow", "testEventW", "countReset", "rTestAddr", "swComm", "berrFlag", "selAddr", "softReset", "blkend", "progResetMod", "busError", "align64", "mode", "offline", "clearData", "overRange", "lowTh", "testACQ", "slideScale", "stepTh", "autoInc", "emptyProg", "slideSub", "allTrigger", /*RO*/"eventCount", "buffEmpty", "buffFull", "CSel0", "CSel1", "DSel0", "DSel1", "DRDY", "gDRDY", "busy", "gbusy", "purged", "termOn", "EVRDY"};

unsigned int confDefault[]={0,0xAA,7,0,0x00CC,0x0000,8,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,8,0,0,4,0,0,0,/*BitSet1*/0,0,0,/*Ctrl1*/0,0,0,0,/*BitSet2*/0,0,0,1,1,0,1,1,1,0,0,1};


adc1785::adc1785(const string& n): smBase(n) {
}

adc1785::~adc1785() {
}

int adc1785::InitializedLOAD(int para) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return 2;
}

int adc1785::LoadedUNLD(int para) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return 1;
}

int adc1785::LoadedCONF(int para) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //if(!configAdc())
        //return -1;
    return 3;
}

int adc1785::ConfiguredUNCF(int para) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseAdc();
    return 2;
}

int adc1785::ConfiguredPREP(int para) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    //prepAdc();
    return 4;
}
int adc1785::ReadySATR(int para) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    //startAdc();
    return 5;
}

int adc1785::RunningSPTR(int para) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int adc1785::ReadySTOP(int para) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    //finishAdc();
    return 3;
}

int adc1785::configAdc() {
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

    base = ((confDefault[AddrHigh]&ADC1785_AddrHigh_Mask)<<24) + ((regValue[AddrLow]&ADC1785_AddrLow_Mask)<<16);
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
    for(i=0; i<confSize-confRO; i++)
        confValue[i]=confDefault[i];
    
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
    for(i=0; i<confSize-confRO; i++) {
        if((res=cfgInfo->infoGetUint("config."+name+"."+confName[i], adcReg)) != 1) {
            debugMsg << name << "# " << "config."+name+"."+confName[i] << " not found.";
            stMsg->stateOut(debugMsg);
            //return 0;
        }
        else
            confValue[i] = adcReg;

        debugMsg << name << "# " << "" << confName[i] << ": " << confValue[i];
        stMsg->stateOut(debugMsg);
    }

    // merge to register
    for(i=0; i<regSize-regRO-regBitSet; i++) {
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
    // D16 to D32
    uint32_t lsi0_ctl = pvme->readUniReg(0x100);
    lsi0_ctl &= 0xFF3FFFFF;
    lsi0_ctl |= D32;
    pvme->writeUniReg(0x100, lsi0_ctl);
    return 1;
}

int adc1785::finishAdc() {
    // D32 to D16
    return 1;
}

int adc1785::startAdc() {
    // clear data
    pvme->ww(image, base+ADC1785_BitSet2_Offset, pvme->swap16((uint16_t)0x0004));
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16((uint16_t)0x0004));

    // enable adc
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before enable adc: " << hex << regBitSet2;
    stMsg->stateOut(debugMsg);
    regBitSet2 &= 0x0002;
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(regBitSet2));

    return 1;
}

int adc1785::stopAdc() {
    debugMsg << name << "# " << "flush " << confValue[eventTh] << " events in the buffer.";
    stMsg->stateOut(debugMsg); 
    for(int i=0; i<confValue[eventTh]; i++) {
        pvme->ww(image, base+ADC1785_SWComm_Offset, pvme->swap16(0x0001));
    }
    
    // disable adc
    uint16_t regBitSet2;
    pvme->rw(image, base+ADC1785_BitSet2_Offset, &regBitSet2);
    regBitSet2 = pvme->swap16(regBitSet2);
    debugMsg << name << "# " << "regBitSet2 before disable adc: " << hex << regBitSet2;
    stMsg->stateOut(debugMsg);
    pvme->ww(image, base+ADC1785_BitClear2_Offset, pvme->swap16(0x0002));

    return 1;
}

int adc1785::getStatus() {
    return 1;
}

int adc1785::clearBuff() {
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new adc1785(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
