#include "dataPacker.h"

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>

using std::string;
using std::thread;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2


dataPacker::dataPacker(const string& n): smBase(n) {
}

dataPacker::~dataPacker() {
}

int dataPacker::ConfiguredPREP(int para) {
    devMsgQue = dataPool->getDevMsgQue();
    netMsgQue = dataPool->getNetMsgQue();
    return 4;
}

int dataPacker::ReadySATR(int para) {
    stMsg->stateOut(1, "dataPacker ReadySATR");
    startPacker();
    return 5;
}

int dataPacker::RunningSPTR(int para) {
    stMsg->stateOut(1, "dataPacker RunningSPTR");
    stopPacker();
    return 4;
}

int dataPacker::PausedSPTR(int para) {
    stMsg->stateOut(1, "dataPacker PausedSPTR");
    stopPacker();
    return 4;
}


int dataPacker::startPacker() {
    stMsg->stateOut(1, "start packer.");
    runPackCtrl = TASK_START;
    t0 = new thread(&dataPacker::runPack, this);
    return 1;
}

int dataPacker::stopPacker() {
    stMsg->stateOut(1, "stop packer.");
    runPackCtrl = TASK_STOP;
    t0->join();
    return 1;
}

void dataPacker::runPack() {

    packStatus = TASK_RUN;

    packCount = 0;
    totalPackSize = 0;
    while(1) {
        if((msgrcv(devMsgQue, &packMsg, sizeof(packMsg), 0, 0)) < 0) {
            packStatus = TASK_ERROR;
            break;
        }

        if(packMsg.signal == 2) {
            break;
        }

        unsigned int packTranSize = packMsg.size;
        if(!packData(packTranSize)) {
            packStatus = TASK_ERROR;
            break;
        }
    }

    if(runPackCtrl == TASK_STOP || packStatus == TASK_ERROR) {
        packMsg.signal = 2;
        packMsg.size = 0;
        if((msgsnd(netMsgQue, &packMsg, sizeof(packMsg), 0)) < 0) {
            packStatus = TASK_ERROR;
        }

        if(packStatus == TASK_RUN)
            packStatus = TASK_EXIT;
    }
}

int dataPacker::packData(unsigned int& packSize) {

    unsigned int tmp[18];
    unsigned int tmpIdx;

    dataPool->devSetSnap();
    unsigned int bias = 0;
    void* p;
    unsigned int value;
    unsigned int tranSize = 0;
    for(unsigned int i=0; i<packSize/4; i++,bias+=4) {
        p = dataPool->devGetSnapPtr(bias, 4);
        if(p == NULL)
            return 0;
        value = *(uint32_t*)p;

        // invalid data
        if((value&0x00000007) == 0x00000006) {
            tmpIdx=0;
            continue;
        }
        // header
        if((value&0x00000007) == 0x00000002) {
            memset(tmp, 0, 18*sizeof(unsigned int));
            tmp[0]=value;
            tmpIdx=1;
            continue;
        }
        // valid data
        if((value&0x00000007) == 0x00000000 && tmpIdx > 0 && tmpIdx < 17) {
            tmp[tmpIdx]=value;
            tmpIdx++;
            continue;
        }
        // end of block
        if((value&0x00000007) == 0x00000004 && tmpIdx > 0 && tmpIdx < 18) {
            tmp[tmpIdx]=value;
            tmpIdx++;
            // copy to Output
            dataPool->netWrite(tmp, tmpIdx*4);
            tranSize += tmpIdx*4;
            tmpIdx=0;
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
