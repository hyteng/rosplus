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

#define dmaSize 8

vme::vme(const string& n): smBase(n) {    
}

vme::~vme() {
}

int vme::InitializedLOAD(int para) {
    //pvme = new VMEBridge;
    return 2;
}

int vme::LoadedUNLD(int para) {
    //delete pvme;
    return 1;
}

int vme::LoadedCONF(int para) {
    //configVme();
    return 3;
}

int vme::ConfiguredUNCF(int para) {
    //releaseVme();
    return 2;
}

int vme::ConfiguredPREP(int para) {
    devMsgQue = dataPool->getDevMsgQue();
    //prepVme();
    return 4;
}
int vme::ReadySATR(int para) {
    stMsg->stateOut(1, "vme ReadySATR");
    startVme();
    return 5;
}

int vme::RunningSPTR(int para) {
    stMsg->stateOut(1, "vme RunningSPTR");
    stopVme();
    return 4;
}

int vme::ReadySTOP(int para) {
    stMsg->stateOut(1, "vme ReadySPTR");
    return 3;
}

int vme::configVme() {
    pvme->resetDriver();
    int res;
 
    uint32_t PCI_CSR, MISC_CTL, MAST_CTL, LSI0_CTL;
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name+"# " << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);
    if(PCI_CSR&0xF9000000)
        pvme->writeUniReg(0x004, 0xF9000007);
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name+"# " << "UniverseII Register after writing 0xF9000007 to register 0x004:" << endl;
    debugMsg << name+"# " << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);

    if((res=cfgInfo->infoGetUint("config."+name+".dmaNumber", dmaNumber)) != 1) {
        debugMsg << name+"# " << "config."+name+".dmaNumber not found.";
        stMsg->stateOut(debugMsg);
        dmaNumber = 1;
    }
    dmaBase = pvme->requestDMA(dmaNumber);
    pvme->setOption(DMA, BLT_ON);

    return 1;
}

int vme::releaseVme() {
    pvme->releaseDMA();
    return 1;
}

int vme::prepVme() {
    // recover the 16bit data transfer to 32bit mode
    uint32_t lsi0_ctl = pvme->readUniReg(0x100);
    lsi0_ctl &= 0xFF3FFFFF;
    lsi0_ctl |= D32;
    pvme->writeUniReg(0x100, lsi0_ctl);
    return 1;
}

int vme::startVme() {
    stMsg->stateOut(1, "start Vme.");
    runVmeCtrl = TASK_START;
    t0 = new thread(&vme::runVme, this);
    return 1;
}

int vme::stopVme() {
    stMsg->stateOut(1, "stop Vme.");
    runVmeCtrl = TASK_STOP;
    t0->join();
    return 1;
}

void vme::runVme() {

    stMsg->stateOut(1, "running Vme.");
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
        */
        //dataPool->devWrite(dmaBase+offset, dmaSize);
        //sleep(1);
        unsigned int tranSize = dataPool->devWrite(tmp, dmaSize);

        genSize += 8;
        sndSize += tranSize;
        debugMsg << "vme send " << genSize << " data, devMsg send " << sndSize << endl;
        stMsg->stateOut(debugMsg);

        //vmeCount++;
        //totalVmeSize += dmaSize;
    }

    if(runVmeCtrl == TASK_STOP || vmeStatus == TASK_ERROR) {
        vmeMsg.signal = 2;
        vmeMsg.size = 0;
        int stopSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg), 0);
        if(stopSend < 0) {
            vmeStatus = TASK_ERROR;
        }

        debugMsg << "vme send stop to devMsg and return " << stopSend << endl;
        stMsg->stateOut(debugMsg);

        if(vmeStatus == TASK_RUN)
            vmeStatus = TASK_EXIT;
    }

    debugMsg << "vme stop thread" << vmeStatus << endl;
    stMsg->stateOut(debugMsg);
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new vme(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
