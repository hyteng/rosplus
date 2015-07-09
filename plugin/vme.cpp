#include "vme.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

using std::string;
using std::thread;
using std::cout;
using std::endl;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2

#define DMASIZE 72

vme::vme(const string& n): smBase(n) {    
}

vme::~vme() {
}

int vme::InitializedLOAD(void* argv[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg); 
    //pvme = new VMEBridge;
    return 2;
}

int vme::LoadedUNLD(void* argv[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    //delete pvme;
    return 1;
}

int vme::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //configVme();
    return 3;
}

int vme::ConfiguredUNCF(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseVme();
    return 2;
}

int vme::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    //if(!prepVme())
        //return -1;
    return 4;
}

int vme::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    //finishVme();
    return 3;
}

int vme::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    startVme();
    return 5;
}

int vme::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    stopVme();
    return 4;
}

int vme::RunningPAUS(void* argv[]) {
    return 6;
}

int vme::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    stopVme();
    return 4;
}

int vme::PausedRESU(void* argv[]) {
    return 5;
}

int vme::OTFCONF(void* argv[]) {
    return stId;
}

int vme::configVme() {
    pvme->resetDriver();
    int res;
 
    uint32_t PCI_CSR, MISC_CTL, MAST_CTL, LSI0_CTL;
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name << "# " << std::hex << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);
    if(PCI_CSR&0xF9000000)
        pvme->writeUniReg(0x004, 0xF9000007);
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name << "# " << "UniverseII Register after writing 0xF9000007 to register 0x004:" << endl;
    debugMsg << name << "# " << std::hex << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);

    if((res=cfgInfo->infoGetUint("config."+name+".dmaNumber", dmaNumber)) != 1) {
        debugMsg << name << "# " << "config."+name+".dmaNumber not found.";
        stMsg->stateOut(debugMsg);
        dmaNumber = 1;
    }
    dmaBase = pvme->requestDMA(dmaNumber);
    pvme->setOption(DMA, BLT_ON);

    if((res=cfgInfo->infoGetUint("config."+name+".dmaSize", dmaSize)) != 1) {
        debugMsg << name << "# " << "config."+name+".dmaSize not found.";
        stMsg->stateOut(debugMsg);
        dmaSize = DMASIZE;
    }
    return 1;
}

int vme::releaseVme() {
    pvme->releaseDMA();
    return 1;
}

int vme::prepVme() {

    adc0Base = -1;
    string adcModeName;
    int res;
    if((res=cfgInfo->infoGetString("config."+name+".adcModeName", adcModeName)) == 1) {
        std::vector< std::pair<std::string, smBase*> >::const_iterator iter;
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == adcModeName)
                adc0Base = *(uint32_t*)(iter->second->getHelp());
        }
    }
    if(adc0Base == -1) {
        debugMsg << name << "# " << "helper " << adcModeName << " not found.";
        stMsg->stateOut(debugMsg);
        return 0;
    }

    devMsgQue = dataPool->getDevMsgQue();
    return 1;
}

int vme::finishVme() {
    return 1;
}

int vme::startVme() {
    runVmeCtrl = TASK_START;
    t0 = new thread(&vme::runVme, this);
    return 1;
}

int vme::stopVme() {
    runVmeCtrl = TASK_STOP;
    //t0->join();
    t0->detach();
    return 1;
}

void vme::runVme() {
    vmeStatus = TASK_RUN;

    vmeCount = 0;
    totalVmeSize = 0;
    unsigned int genSize = 0;
    unsigned int sndSize = 0;
    char *tmp = "ABCDEFGH";
    while(1) {

        if(runVmeCtrl == TASK_STOP) {
            break;
        }
        /*       
        pvme->waitIrq(7, 0);
        int offset = pvme->DMAread(adc0Base, dmaSize, A32, D32);
        if(offset < 0) {
            vmeStatus = TASK_ERROR;
            break;
        }
        unsigned int tranSize = dataPool->devWrite((void*)(dmaBase+offset), dmaSize);
        */
        sleep(1);
        unsigned int tranSize = dataPool->devWrite(tmp, dmaSize);

        genSize += dmaSize;
        sndSize += tranSize;
        //vmeCount++;
        //totalVmeSize += dmaSize;
        debugMsg << name << "# " << "vme generate " << dmaSize << " bytes data, pool write " << tranSize << " bytes. total gen " << genSize << ", total send " << sndSize << " bytes";
        stMsg->stateOut(debugMsg);
    }

    if(runVmeCtrl == TASK_STOP || vmeStatus == TASK_ERROR) {
        vmeMsg.signal = 2;
        vmeMsg.size = 0;
        int stopSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg), 0);
        if(stopSend < 0) {
            vmeStatus = TASK_ERROR;
        }

        debugMsg << name << "# " << "vme send stop to devMsg and return " << stopSend << endl;
        stMsg->stateOut(debugMsg);

        if(vmeStatus == TASK_RUN)
            vmeStatus = TASK_EXIT;
    }

    debugMsg << name << "# " << "vme stop thread" << vmeStatus << endl;
    stMsg->stateOut(debugMsg);
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new vme(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
