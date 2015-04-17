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
#define TASK_ERROR 2

struct taskMsg {
    long signal;
    unsigned int size;
    unsigned int count;
};

daq::daq(const string& n): smBase(n) {
    dmaRing = new ringBuffer();
    netRing = new ringBuffer();
}

daq::~daq() {
    delete dmaRing;
    delete netRing;
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

    if(dmaRing->isVaild())
        dmaRing->release();
    if(netRing->isVaild())
        netRing->release();
    dmaRing->create(dmaRingSize);
    netRing->create(netRingSize);

    key_t dmaKey, netKey;

    if((dmaKey=ftok(".", 'vmeDma')) == -1) {
        return 0;
    }
    if((dmaMsgQue=msgget(dmaKey, 0)) >= 0) {
        if((msgctl(dmaMsgQue, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((dmaMsgQue=msgget(dmaKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return 0;
    }

    if((netKey=ftok(".", 'vmeNet')) == -1) {
        return 0;
    }
    if((netMsgQue=msgget(netKey, 0)) >= 0) {
        if((msgctl(dmaMsgQue, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((netMsgQue=msgget(netKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
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
    runPackCtrl = TASK_START;
    runNetCtrl = TASK_START;
    t0 = new thread(&daq::runDma, this);
    t1 = new thread(&daq::runPack, this);
    t2 = new thread(&daq::runNet, this);
    return 1;
}

int daq::stopDaq() {

    runDmaCtrl = TASK_STOP;
    runPackCtrl = TASK_STOP;
    runNetCtrl = TASK_STOP;
    stMsg->stateOut(1, "stop Daq.");
    return 1;
}

void daq::runDma(void *para) {

    dmaStatus = TASK_RUN;

    totalDmaSize = 0;
    dmaCount = 0;
    struct taskMsg dmaMsg;
    unsigned int buffCount = 0;
    while(1) {
        if(runDmaCtrl == TASK_STOP && totalDmaSize == 0) {
            break;
        }

        /*
        pvme->waitIrq(irqLevel, statusID);
        unsigned int offset = pvme->DMAread(adc0.base, dmaTranSize, A32, D32);
        if(offset < 0) {
            dmaStatus = TASK_ERROR;
            break;
        }
        */

        buffCount++;
        
        //if((msgrcv(dmaMsgQue, &dmaMsg, sizeof(dmaMsg), 0, 0)) < 0) {}

        dmaRing->dmaWrite(dma.base+offset, dmaTranSize);
        dmaCount++;
        totalDmaSize += dmaTranSize;

        if((totalDmaSize+dmaTranSize) >= packSize || runDmaCtrl == TASK_STOP) {
            dmaMsg.signal = 1;
            dmaMsg.size = totalDmaSize;
            dmaMsg.count = buffCount;
            if((msgsnd(dmaMsgQue, &dmaMsg, sizeof(dmaMsg), 0)) < 0) {
                dmaStatus = TASK_ERROR;
                break;
            }
            totalDmaSize = 0;
            buffCount = 0;
        }
    }

    if(runDmaCtrl == TASK_STOP || dmaStatus == TASK_ERROR) {
        runPackCtrl = TASK_STOP;
        dmaMsg.signal = 2;
        dmaMsg.size = 0;
        if((msgsnd(dmaMsgQue, &dmaMsg, sizeof(dmaMsg), 0)) < 0) {
            dmaStatus = TASK_ERROR;;
        }

        if(dmaStatus == TASK_RUN)
            dmaStatus = TASK_EXIT;
    }
}

void daq::runPack(void* para) {

    packStatus = TASK_RUN;

    struct taskMsg dmaMsg;
    struct taskMsg netMsg;
    packCount = 0;
    totalPackSize = 0;
    void* tmpBuff0 = malloc(packSize);
    void* tmpBuff1 = malloc(packSize);
    while(1) {
        //if(runPackCtrl == TASK_STOP && totalPackSize == 0 && dmaCount == 0) {
        if((msgrcv(dmaMsgQue, &dmaMsg, sizeof(dmaMsg), 0, 0)) < 0) {
            packStatus = TASK_ERROR;
            break;
        }

        if(dmaMsg.signal == 2) {
            break;
        }

        unsigned int packTranSize = dmaMsg.size;
        dmaRing->dmaRead(tmpBuff0, packTranSize);
        dmaCount -= dmaMsg.count;
        packData(tmpBuff0, tmpBuff1, packTranSize);

        if(packTranSize != 0) {
            netMsg.signal = 1;
            netMsg.size = packTranSize;
            netMsg.count = 1;

            packCount++;
            totalPackSize += packTranSize;
            netRing->dmaWrite(tmpBuff1, packTranSize);
            if((msgsnd(netMsgQue, &netMsg, sizeof(netMsg), 0)) < 0) {
                packStatus = TASK_ERROR;
                break;
            }
            totalPackSize = 0;
        }
    }

    if(runPackCtrl == TASK_STOP || packStatus == TASK_ERROR) {
        //runNetCtrl = TASK_STOP;
        netMsg.signal = 2;
        netMsg.size = 0;
        if((msgsnd(netMsgQue, &netMsg, sizeof(netMsg), 0)) < 0) {
            packStatus = TASK_ERROR;
        }

        if(packStatus == TASK_RUN)
            packStatus = TASK_EXIT;
    }

    free((void*)tmpBuff0);
    free((void*)tmpBuff1);
}

void daq::runNet(void *para) {
    netStatus = TASK_RUN;

    generateFileHeader();

    unsigned int buffCount = 0;
    unsigned int buffSize = 0;
    struct taskMsg netMsg;
    while(1) {
        if(runNetCtrl != TASK_STOP)
            if((msgrcv(netMsgQue, &netMsg, sizeof(netMsg), 0, 0)) < 0) {
                netStatus = TASK_ERROR;
                break;
            }

        if(netMsg.signal == 2)
            runNetCtrl = TASK_STOP;

        if(runNetCtrl == TASK_STOP && packCount == 0) {
            break;
        }

        buffCount += netMsg.count;
        buffSize += netMsg.size;

        if(buffCount >= 5 || runNetCtrl == TASK_STOP) {
            netRing->dmaRead(filePtr, buffSize);
            packCount -= buffCount;
            buffCount = 0;
            buffSize = 0;
        }
    }

    if(runNetCtrl == TASK_STOP) {
        if(netStatus == TASK_RUN)
            netStatus = TASK_EXIT;
    }
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
