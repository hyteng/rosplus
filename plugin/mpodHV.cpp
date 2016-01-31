#include "mpodHV.h"
#include <algorithm>
#include <string.h>
#include <unistd.h>

#ifdef SNMP_PP_NAMESPACE                                                        
using namespace Snmp_pp;                                                        
#endif

#define SYSDESCR "1.3.6.1.2.1.1.1.0"    // Object ID for System Descriptor

using std::string;
using std::stringstream;
using std::map;
using std::vector;
using std::cout;
using std::endl;
using std::hex;
using std::dec;


static map<string, vector<string> > mpodHV_ctrl2conf;
static map<string, int> mpodHV_conf2reg;
static map<string, uintptr_t> mpodHV_conf2mask;
static vector<uintptr_t> mpodHV_regAddr;
static vector<int> mpodHV_regRWIdx;

static int setCtrl() {
    mpodHV_ctrl2conf.clear();
    for(int i=0,j=0; i<ctrlSize; i++, j++) {
        mpodHV_ctrl2conf[ctrl1785[i]] = vector<string>(0);
        mpodHV_ctrl2conf[ctrl1785[i]].push_back(confName1785[j]);
        
        if(i==ctrlReg+ctrlBitSet1-1)
            j+=ctrlBitSet1;

        if(i>=ctrlReg+ctrlBitSet1+ctrlCtrl1 && i<ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2)
            mpodHV_ctrl2conf[ctrl1785[i]].push_back(confName1785[j+ctrlBitSet2]);
        if(i==ctrlReg+ctrlBitSet1+ctrlCtrl1+ctrlBitSet2-1)
            j+=ctrlBitSet2;
    }

    mpodHV_conf2reg.clear();
    mpodHV_conf2mask.clear();
    for(int i=0, j=0; i<confSize; i++,j++) {
        if(i>=confReg && i<confReg+confBitSet1)
            j = confReg;

        mpodHV_conf2reg[confName1785[i]] = j;
        mpodHV_conf2mask[confName1785[i]] = (uintptr_t)(&confMask1785[i]);
    }

    mpodHV_regAddr.clear();
    mpodHV_regRWIdx.clear();
    for(int i=0; i<regSize; i++) {
        mpodHV_regAddr.push_back((uintptr_t)(&regAddr1785[i]));
        int rw = 0;
        if(i>=regRW+regWO+regBitSet)
            rw = 1;
        mpodHV_regRWIdx.push_back(rw);
    }

    return 1;
}

static int dummy = setCtrl();


mpodHV::mpodHV(const string& n): smBase(n) {
    ctrl2conf = &mpodHV_ctrl2conf;
    ctrl2level = &mpodHV_ctrl2level;
    conf2reg = &mpodHV_conf2reg;
    conf2mask = &mpodHV_conf2mask;
    regAddr = &mpodHV_regAddr;
    regRWIdx = &mpodHV_regRWIdx;

    vd = (regData*) new regUint16();
    vm = (regData*) new regUint16();
}

mpodHV::~mpodHV() {
}

int mpodHV::queryInterface(const std::string& funcName, void* para[], void* ret) {
    if(funcName == "fillEvent") {
        *(int*)ret = fillEvent(*(unsigned int*)para[0]);
        return 1;
    }
    return 0;
}

int mpodHV::InitializedLOAD(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg);
    return smBase::InitializedLOAD(ret, para);
}

int mpodHV::LoadedUNLD(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    return smBase::LoadedUNLD(ret, para);
}

int mpodHV::LoadedCONF(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    if(!configMPod(ret))
        return -1;
    ret = "3;"+ret;
    stId = 3;
    return 3;
}

int mpodHV::ConfiguredUNCF(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    releaseMPod();
    return smBase::ConfiguredUNCF(ret, para);
}

int mpodHV::ConfiguredPREP(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepMPod();
    return smBase::ConfiguredPREP(ret, para);
}

int mpodHV::ReadySTOP(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    finishMPod();
    return smBase::ReadySTOP(ret, para);
}

int mpodHV::ReadySATR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    return smBase::ReadySATR(ret, para);
}

int mpodHV::RunningSPTR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    return smBase::RunningSPTR(ret, para);
}

int mpodHV::RunningPAUS(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "RunningPAUS";
    stMsg->stateOut(debugMsg);
    return smBase::RunningPAUS(ret, para);
}

int mpodHV::PausedSPTR(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    return smBase::PausedSPTR(ret, para);
}

int mpodHV::PausedRESU(std::string& ret, void* para[]) {
    debugMsg << name << "# " << "PausedRESU";
    stMsg->stateOut(debugMsg);
    return smBase::PausedRESU(ret, para);
}

int mpodHV::accessReg(const int idx, const int rw, regData& data) {
    int res;
    if(idx<0 || idx>=regSize || rw<0 || rw>1 || data.getValueP()==NULL )
        return 0;

    if((*regRWIdx)[idx] != 0 && (*regRWIdx)[idx] != rw+1)
        return 0;

    regAddrType addr = *(regAddrType*)((*regAddr)[idx]);
    
    res = accessRegNormal(addr, rw, (regType*)data.getValueP());
    return res;
}

int mpodHV::maskRegData(regData& data, regData& mask) {
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

int mpodHV::unmaskRegData(regData& data, regData& mask) {
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

int mpodHV::configMPod() {

    CTarget ctarget((IpAddress)"192.168.1.100");

    int status;
    snmp = new Snmp(status);    // create a SNMP++ session 
    if(status != SNMP_CLASS_SUCCESS) {    // check creation status
        cout << snmp->error_msg(status);    // if fail, print error string
        return 0; 
    }

    vb.set_oid("1.3.6.1.4.1.19947.1");// get next starting seed
    pdu += vb;// add vb to the pdu

    string oidLevel;
    int len = std::string("1.3.6.1.4.1.19947.1.7.6.0").length();
    status = SNMP_CLASS_SUCCESS;
    while(status == SNMP_CLASS_SUCCESS) {
        if((status = snmp->get_next(pdu, ctarget)) == SNMP_CLASS_SUCCESS) {
            pdu.get_vb(vb,0);
            oidLevel = std::string(vb.get_printable_oid());
            if(oidLevel.length() >= len)
                if(oidLevel.substr(0,len)=="1.3.6.1.4.1.19947.1.7.6.0")
                    break;
            cout << "Mib Object = " << vb.get_printable_oid() << endl;
            cout << "Mib Value = " << vb.get_printable_value() << endl;
            pdu.set_vb(vb,0);
        }
        else
            cout << "SNMP++ Error = " << snmp->error_msg(status);
    }

    return 1;
}

int releaseMPod() {

    delete snmp;
    return 1
}

