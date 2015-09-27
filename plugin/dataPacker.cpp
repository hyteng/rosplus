#include "dataPacker.h"
#include <thread>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h> // for memset function

using std::stringstream;
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

int dataPacker::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepPacker();
    return 4;
}

int dataPacker::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    startPacker();
    return 5;
}

int dataPacker::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    stopPacker();
    return 4;
}

int dataPacker::RunningPAUS(void* argv[]) {
    return 6;
}

int dataPacker::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    stopPacker();
    return 4;
}

int dataPacker::PausedRESU(void* argv[]) {
    return 5;
}

int dataPacker::OTFCONF(void* argv[]) {
    return stId;
}

int dataPacker::prepPacker() {

    string vmeDevice;
    int res;
    std::vector< std::pair<std::string, smBase*> >::const_iterator iter;

    vmeDev = NULL;
    if((res=cfgInfo->infoGetString("config."+name+".vmeDev", vmeDevice)) == 1) {
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == vmeDevice) {
                vmeDev = iter->second;
                vmeList.clear();
                if(!vmeDev->queryInterface("getDevList", NULL, (void*)&vmeList))
                    return 0;
                vmeSize = vmeList.size();
            }
        }
    }

    // the pack list must contain the same device sequence from vme module and put those in the front of the vector
    string packListAll;
    if((res=cfgInfo->infoGetString("config."+name+".packList", packListAll)) == 1) {
        packStringSplit(packListAll);
    }

    packList.clear();
    packList.resize(listSize);
    for(int i=0; i<listSize; i++) {
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == devList[i]) {
                packList[i] = iter->second;
            }
        }
    }

    if(vmeDev == NULL || listSize == 0) {
        debugMsg << name << "# " << "helper or link List not found.";
        stMsg->stateOut(debugMsg);
        return 0;
    }

    devMsgQue = dataPool->getDevMsgQue();
    netMsgQue = dataPool->getNetMsgQue();
    return 1;
}

int dataPacker::packStringSplit(const string& pList) {
    stringstream sList(pList);
    string dev;
    debugMsg << name << "# " << "vme take device list" << pList << endl;
    stMsg->stateOut(debugMsg);
    devList.clear();
    listSize = 0;
    while(getline(sList, dev, ';')) {
        if(listSize < vmeSize && dev != vmeList[listSize]) {
            debugMsg << name << "# " << "vme list do not contain the device" << dev << endl;
            devList.clear();
            listSize = 0;
            break;
        }
        devList.push_back(dev);
        listSize++;
        debugMsg << name << "# " << "dev link " << listSize << ", " << dev << endl;
        stMsg->stateOut(debugMsg);
    }
    return listSize;
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
    dataSize.clear();
    unsigned int recSize = 0;
    unsigned int sndSize = 0;
    while(1) {
        if((msgrcv(devMsgQue, &packMsg, sizeof(packMsg), 0, 0)) < 0) {
            packStatus = TASK_ERROR;
            break;
        }

        //debugMsg << name << "# " << "fetch devMsg " << packMsg.size;
        //stMsg->stateOut(debugMsg);
        if(packMsg.signal == 0) {
            packCount += packMsg.count;
            totalPackSize += packMsg.size;
            dataSize.push_back(packMsg.count);
        }

        if(packMsg.signal == 1) {
            eventTh = packMsg.size;
            unsigned int packTranSize = totalPackSize;
            if(!packData(packTranSize)) {
                packStatus = TASK_ERROR;
                break;
            }

            int packSend = msgsnd(netMsgQue, &packMsg, sizeof(packMsg)-sizeof(long), 0);
            if(packSend < 0) {
                packStatus = TASK_ERROR;
                break;
            }
    
            recSize += totalPackSize;
            sndSize += packTranSize;
            debugMsg << name << "# " << "read" << recSize << "data and send " << sndSize << endl;
            stMsg->stateOut(debugMsg);

            packCount = 0;
            totalPackSize = 0;
            dataSize.clear();
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

    unsigned int tranSize = 0, tmpSize;
    int ret = 0;
    int dSize = dataSize.size();
    void* pSize;
    for(int i=0; i<listSize; i++) {
        if(i < dSize)
            pSize = (void*)&dataSize[i];
        else
            pSize = (void*)&tmpSize;
        packList[i]->queryInterface("packData", &pSize, (void*)&ret);
        tranSize += *(unsigned int*)pSize;
    }

    // device in packList could be more than device in vme module, since some device need not to transfer data through DMA, which are arranged to the end of packing sequence
    tranSize = 0;
    for(int i=0; i<eventTh; i++) {
        fillEvent(tmpSize);
        tranSize += tmpSize;
        pSize = (void*)&tmpSize;
        for(int j=0; j<listSize; j++) {
            packList[j]->queryInterface("fillEvent", &pSize, (void*)&ret);
            tranSize += *(unsigned int*)pSize;
        }
    }

    packSize = tranSize;
    return 1;
}

int dataPacker::packDataTest1(unsigned int& packSize) {

    uint32_t tmp[18];
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
        dataPool->netWrite(p, 4);
        tranSize += 4;
    }

    unsigned int popSize = dataPool->devPopSnap(packSize);
    if(popSize != packSize)
        return 0;

    packSize = tranSize;
    return 1;
}

int dataPacker::packDataTest2(unsigned int& packSize) {

    uint32_t tmp[18];
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

int dataPacker::fillEvent(unsigned int& packSize) {
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new dataPacker(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
