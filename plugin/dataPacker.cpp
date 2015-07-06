#include "dataPacker.h"

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>

using std::string;
using std::thread;
using std::cout;
using std::endl;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2


dataPacker::dataPacker(const string& n): smBase(n) {
}

dataPacker::~dataPacker() {
}

int dataPacker::ConfiguredPREP(int argc, void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepPacker();
    return 4;
}

int dataPacker::ReadySATR(int argc, void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    startPacker();
    return 5;
}

int dataPacker::RunningSPTR(int argc, void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    stopPacker();
    return 4;
}

int dataPacker::RunningPAUS(int argc, void* argv[]) {
    return 6;
}

int dataPacker::PausedSPTR(int argc, void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    stopPacker();
    return 4;
}

int dataPacker::PausedRESU(int argc, void* argv[]) {
    return 5;
}

int dataPacker::OTFCONF(int argc, void* argv[]) {
    return stId;
}

int dataPacker::prepPacker() {
    devMsgQue = dataPool->getDevMsgQue();
    netMsgQue = dataPool->getNetMsgQue();
    return 1;
}

int dataPacker::startPacker() {
    runPackCtrl = TASK_START;
    t0 = new thread(&dataPacker::runPack, this);
    return 1;
}

int dataPacker::stopPacker() {
    runPackCtrl = TASK_STOP;
    //t0->join();
    return 1;
}

void dataPacker::runPack() {

    packStatus = TASK_RUN;

    packCount = 0;
    totalPackSize = 0;
    unsigned int recSize = 0;
    unsigned int sndSize = 0;
    while(1) {
        if((msgrcv(devMsgQue, &packMsg, sizeof(packMsg), 0, 0)) < 0) {
            packStatus = TASK_ERROR;
            break;
        }

        //debugMsg << name << "# " << "fetch devMsg " << packMsg.size;
        //stMsg->stateOut(debugMsg);

        packCount += packMsg.count;
        totalPackSize += packMsg.size;
        if(packMsg.signal == 2 || totalPackSize >= 72) {
 
            unsigned int packTranSize = totalPackSize;
            if(!packData(packTranSize)) {
                packStatus = TASK_ERROR;
                break;
            }
            
            recSize += totalPackSize;
            sndSize += packTranSize;
            debugMsg << name << "# " << "read" << recSize << "data and send " << sndSize << endl;
            stMsg->stateOut(debugMsg);

            packCount = 0;
            totalPackSize = 0;
        }

        if(packMsg.signal == 2) {
            break;
        }
    }

    //if(runPackCtrl == TASK_STOP || packStatus == TASK_ERROR) {
        packMsg.signal = 2;
        packMsg.size = 0;
        int stopSend = msgsnd(netMsgQue, &packMsg, sizeof(packMsg)-sizeof(long), 0);
        if(stopSend < 0) {
            std::cout << "pack msg fail"  << std::endl;
            packStatus = TASK_ERROR;
        }

        debugMsg << name << "# " << "send stop to netMsg and return " << stopSend;
        stMsg->stateOut(debugMsg);

        if(packStatus == TASK_RUN)
            packStatus = TASK_EXIT;
    //}

    debugMsg << name << "# " << "stop thread" << packStatus;
    stMsg->stateOut(debugMsg);
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
            debugMsg << name << "# " << "pack data: " << endl;
            for(int i=0; i<tmpIdx; i++) {
                debugMsg << std::hex << tmp[i] << " ";
            }
            stMsg->stateOut(debugMsg);
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

int dataPacker::packDataTest(unsigned int& packSize) {

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
        /*
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
        */
        dataPool->netWrite(p, 4);
        tranSize += 4;
    }

    unsigned int popSize = dataPool->devPopSnap(packSize);
    if(popSize != packSize)
        return 0;

    packSize = tranSize;
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new dataPacker(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
