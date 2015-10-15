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
        debugMsg << name << "# " << "helper get " << vmeDevice;
        stMsg->stateOut(debugMsg);
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == vmeDevice) {
                vmeDev = iter->second;
                vmeList.clear();
                if(!vmeDev->queryInterface("getNameList", NULL, (void*)&vmeList))
                    return 0;
                vmeSize = vmeList.size();
                debugMsg << name << "# " << "helper get " << vmeDevice << " and get " << vmeSize << " from its devList.";
                stMsg->stateOut(debugMsg);
            }
        }
    }

    // the pack list must contain the same device sequence from vme module and put those in the front of the vector
    string packListAll;
    if((res=cfgInfo->infoGetString("config."+name+".packList", packListAll)) == 1) {
        packStringSplit(packListAll);
    }

    if(vmeDev == NULL || listSize == 0) {
        debugMsg << name << "# " << "helper or link List not found.";
        stMsg->stateOut(debugMsg);
        return 0;
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
    dataSize.clear();
    unsigned int recSize = 0;
    unsigned int sndSize = 0;
    while(1) {
        if((msgrcv(devMsgQue, &packMsg, sizeof(packMsg), 0, 0)) < 0) {
            packStatus = TASK_ERROR;
            break;
        }

        debugMsg << name << "# " << "fetch devMsg " << packMsg.size;
        stMsg->stateOut(debugMsg);
        if(packMsg.signal == 1) {
            packCount += packMsg.count;
            totalPackSize += packMsg.size;
            dataSize.push_back(packMsg.size);
            continue;
        }

        if(packMsg.signal == 2) {
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

            continue;
        }

        if(packMsg.signal == 3) {
            break;
        }
    }

    //if(runPackCtrl == TASK_STOP || packStatus == TASK_ERROR) {
        packMsg.signal = 3;
        packMsg.size = 0;
        int stopSend = msgsnd(netMsgQue, &packMsg, sizeof(packMsg)-sizeof(long), 0);
        if(stopSend < 0) {
            debugMsg << name << "# " << "pack msg fail";
            stMsg->stateOut(debugMsg);
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


int dataPacker::packData(unsigned int& packSize) {
    unsigned int tranSize = 0, tmpSize;
    int ret = 0;
    int dSize = dataSize.size();
    void* pSize;
    for(int i=0; i<listSize; i++) {
        if(i < dSize)
            pSize = (void*)&dataSize[i];
        else {
            tmpSize = 0;
            pSize = (void*)&tmpSize;
        }
        debugMsg << name << "# " << "dev " << devList[i] << "before pack " << *(unsigned int*)pSize << ";";
        stMsg->stateOut(debugMsg);
        packList[i]->queryInterface("packData", &pSize, (void*)&ret);
        debugMsg << name << "# " << "dev " << devList[i] << ": after pack " << *(unsigned int*)pSize << "bytes data";
        stMsg->stateOut(debugMsg);
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
            debugMsg << name << "# " << "dev: " << devList[j] << " fill " << *(unsigned int*)pSize << "bytes data to a event";
            stMsg->stateOut(debugMsg);
            tranSize += *(unsigned int*)pSize;
        }
    }
    packSize = tranSize;
    return 1;
}

int dataPacker::packDataTest(unsigned int& packSize) {

    uint32_t tmp[18];
    unsigned int tmpIdx;

    dataPool->devSetSnap();
    unsigned int bias = 0;
    void* p;
    unsigned int value;
    unsigned int tranSize = 0;
    unsigned int wordSize=4, readSize, restSize;
    for(unsigned int i=0; i<packSize/wordSize; i++,bias+=wordSize) {
        readSize = wordSize;
        p = dataPool->devGetSnapPtr(bias, readSize);
        if(p == NULL)
            return 0;
        if(readSize < wordSize) {
            dataPool->netWrite(p, readSize);
            restSize = wordSize-readSize;
            p = dataPool->devGetSnapPtr(bias+readSize, restSize);
            dataPool->netWrite(p, restSize);
        }
        else {
            dataPool->netWrite(p, readSize);
        }
        tranSize += wordSize;
    }

    unsigned int popSize = dataPool->devPopSnap(packSize);
    if(popSize != packSize)
        return 0;

    packSize = tranSize;
    return 1;
}

int dataPacker::fillEvent(unsigned int& packSize) {
    packSize = 0;
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new dataPacker(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
