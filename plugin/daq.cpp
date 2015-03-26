#include "daq.h"
#include <thread>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

using std::string;
using std::thread;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0

struct taskMsg {
    long signal;
    unsigned int size;
};

daq::daq(const string& n): smBase(n) {
    ringDma = new ringBuffer();
    ringNet = new ringBuffer();
}

daq::~daq() {
    delete ringDma;
    delete ringNet;
}

int daq::InitializedLOAD(int para) {
    return 2;
}

int daq::LoadedUNLD(int para) {
    return 1;
}

int daq::LoadedCONF(int para) {
    return 3;
}

int daq::ConfiguredUNCF(int para) {
    return 2;
}

int daq::ConfiguredPREP(int para) {
    return 4;
}

int daq::ReadySATR(int para) {

    stMsg->stateOut(1, "daq SATR");
    return 5;
}

int daq::RunningSPTR(int para) {

    stMsg->stateOut(1, "daq SPTR");
    t0->join();
    return 4;
}

int daq::ReadySTOP(int para) {
    return 3;
}

int daq::RunningPAUS(int para) {
    return 6;
}

int daq::PausedSTOP(int para) {
    return 3;
}

int daq::PausedRESU(int para) {
    return stId;
}

int daq::PausedSATR(int para) {
    return 5;
}

int daq::SelfTrans(int para) {
    return 7;
}

int daq::AnyIMPO(int para) {
    return stId;
}

int daq::AnyEXIT(int para) {
    return 0;
}

int daq::beforeDaq() {

    if(ringDma->isVaild())
        ringDma->release();
    if(ringNet->isVaild())
        ringNet->release();
    ringDma->create(size0);
    ringNet->create(size1);

    key_t keyDma, keyNet;

    if((keyDma=ftok(".", 'vmeDma')) == -1) {
        return 0;
    }
    if((msgQDma=msgget(keyDma, 0)) >= 0) {
        if((msgctl(msgQDma, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((msgQDma=msgget(keyDma, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return 0;
    }

    if((keyNet=ftok(".", 'vmeNet')) == -1) {
        return 0;
    }
    if((msgQNet=msgget(keyNet, 0)) >= 0) {
        if((msgctl(msgQDma, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((msgQNet=msgget(keyNet, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return 0;
    }

    return 1;
}

int daq::afterDaq() {

    ring0->release();
    ring1->release();

    return 1;
}

int daq::startDaq() {

    stMsg->stateOut(1, "start Daq.");
    runDmaCtrl = TASK_START;
    runNetCtrl = TASK_STOP;
    t0 = new thread(&daq::runDma, this);
    t0 = new thread(&daq::runNet, this);
    return 1;
}

int daq::stopDaq() {

    runDmaCtrl = TASK_START;
    runNetCtrl = TASK_STOP;
    stMsg->stateOut(1, "stop Daq.");
    return 1;
}

void daq::runDma(void *para) {

    totalDmaSize = 0;
    struct message msgDma;
    unsigned int buffCount=0, dmaCount=0;
    while(1) {
        if(runDmaCtrl == TASK_STOP && totalDmaSize == 0)
            break;

        pvme->waitIrq(irqLevel, statusID);
        unsigned int offset = pvme->DMAread(adc0.base, dmaTranSize, A32, D32);
        if(offset < 0) {
            stopDaq();
            continue;
        }
        buffCount++;

        ringDma->dmaWrite(dma.base+offset, dmaTranSize);
        dmaCount++;
        buffCount--;
        totalDmaSize += dmaTranSize;

        if(totalDmaSize >= packSize || runDmaCtrl == TASK_STOP) {
            msgDma.signal = 1;
            msgDma.size = totalDmaSize;
            if((msgsnd(msgQDma, &msgDma, sizeof(msgDma), 0)) < 0) {
                stopDaq();
                continue;
            }
            totalDmaSize = 0;
        }
    }
}

void daq::runPack(void* para) {
}

void daq::runNet(void *para) {
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
