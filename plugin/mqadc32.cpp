#include "mqadc32.h"

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



static map<string, vector<string> > mqadc32_ctrl2conf;
static map<string, int> mqadc32_conf2reg;
static map<string, uintptr_t>mqadc32_conf2mask;
static vector<uintptr_t> mqadc32_regAddr;
static vector<int> mqadc32_regWOIdx, mqadc32_regROIdx;



static int setDev() {
    return 1;
}

static int dummy = setDev();

mqadc32::mqadc32(const string& n): smBase(n) {
    ctrl2conf = &mqadc32_ctrl2conf;
    conf2reg = &mqadc32_conf2reg;
    conf2mask = &mqadc32_conf2mask;
    regAddr = &mqadc32_regAddr;
    regWOIdx = &mqadc32_regWOIdx;
    regROIdx = &mqadc32_regROIdx;

    vd = (regData*) new regUint16();
    vm = (regData*) new regUint16();
}

mqadc32::~mqadc32() {
}

int mqadc32::InitializedLOAD(void* argv[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return 2;
}

int mqadc32::LoadedUNLD(void* argv[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return 1;
}

int mqadc32::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //if(!configAdc())
        //return -1;
    return 3;
}

int mqadc32::ConfiguredUNCF(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseAdc();
    return 2;
}

int mqadc32::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    //prepAdc();
    return 4;
}

int mqadc32::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    //finishAdc();
    return 3;
}

int mqadc32::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    //startAdc();
    return 5;
}

int mqadc32::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int mqadc32::RunningPAUS(void* argv[]) {
    debugMsg << name << "# " << "RunningPAUS";
    stMsg->stateOut(debugMsg);
    //disableAdc();
    return 6;
}

int mqadc32::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    //stopAdc();
    return 4;
}

int mqadc32::PausedRESU(void* argv[]) {
    debugMsg << name << "# " << "PausedRESU";
    stMsg->stateOut(debugMsg);
    //enableAdc();
    return 5;
}

int mqadc32::accessReg(const int idx, const int rw, regData& data) {
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

int mqadc32::maskRegData(regData& data, regData& mask) {
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

int mqadc32::unmaskRegData(regData& data, regData& mask) {
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


int mqadc32::accessRegNormal(const regAddrType addr, const int rw, regType* data) {
    int res;
    if(rw == 0)
        res = pvme->rw(image, base+addr, data);
    if(rw == 1)
        res = pvme->ww(image, base+addr, data);
    return res;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new mqadc32(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
