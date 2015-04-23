#include "vme.h"
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

using std::string;
using std::thread;

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

int vme::LoadedCONF(int para) {
    configVme();
    return 3;
}

int vme::ConfiguredPREP(int para) {
    devMsgQue = dataPool->getDevMsgQue();
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
    cfgInfo->infoGetUint("vme.maBase", dmaBase); 
    cfgInfo->infoGetUint("vme.maBase", dmaBase); 
    cfgInfo->infoGetUint("vme.maBase", dmaBase); 
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
    t0->detach();
    return 1;
}

void vme::runVme() {

    stMsg->stateOut(1, "running Vme.");
    vmeStatus = TASK_RUN;

    vmeCount = 0;
    totalVmeSize = 0;
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
        sleep(1);
        unsigned int tranSize = dataPool->devWrite(tmp, dmaSize);
        //vmeCount++;
        //totalVmeSize += dmaSize;
    }

    if(runVmeCtrl == TASK_STOP || vmeStatus == TASK_ERROR) {
        vmeMsg.signal = 2;
        vmeMsg.size = 0;
        if((msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg)-sizeof(long), 0)) < 0) {
            vmeStatus = TASK_ERROR;
        }

        if(vmeStatus == TASK_RUN)
            vmeStatus = TASK_EXIT;
    }
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new vme(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
