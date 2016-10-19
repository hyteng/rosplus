#include "daq.h"

#include <iostream>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

using std::string;
using std::thread;
using std::cout;
using std::endl;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2

static string runHead = "RUN#";
static const void* pHead = runHead.c_str();

daq::daq(const string& n): smBase(n) {
}

daq::~daq() {
}

int daq::LoadedCONF(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "LoadedCONF";
    //stMsg->stateOut(debugMsg);
    configDaq();
    return smBase::LoadedCONF(ret, para);
}

int daq::ConfiguredPREP(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "ConfiguredPREP";
    //stMsg->stateOut(debugMsg);
    prepDaq();
    return smBase::ConfiguredPREP(ret, para);
}

int daq::ReadySTOP(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "ReadySTOP";
    //stMsg->stateOut(debugMsg);
    finishDaq();
    return smBase::ReadySTOP(ret, para);
}

int daq::ReadySATR(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "ReadySATR";
    //stMsg->stateOut(debugMsg);
    startDaq();
    return smBase::ReadySATR(ret, para);
}

int daq::RunningSPTR(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "RunningSPTR";
    //stMsg->stateOut(debugMsg);
    stopDaq();
    return smBase::RunningSPTR(ret, para);
}

int daq::PausedSPTR(std::string& ret, void* para[]) {
    //debugMsg << name << "# " << "PausedSPTR";
    //stMsg->stateOut(debugMsg);
    stopDaq();
    return smBase::PausedSPTR(ret, para);
}

int daq::configDaq() {
    fileName = "data_sample";
    if(!cfgInfo->infoGetString("config."+name+".fileName", fileName)) {
        debugMsg << name << "# " << "config: could not get config."+name+".fileName";
        stMsg->stateOut(debugMsg);
    }
    fileIdx = 0;
    return 1;
}

int daq::prepDaq() {
    netMsgQue = dataPool->getNetMsgQue();
    dataPool->netClear();

    fileIdx++;
    std::stringstream sIdx;
    sIdx << fileIdx;
    theFileName = fileName + sIdx.str() + ".txt";
    outFile.close();
    outFile.open(theFileName, std::ios::out|std::ios::binary|std::ios::trunc);
    outFile.write((const char*)pHead, 4);
    return 1;
}

int daq::finishDaq() {
    outFile.close();
    return 1;
}

int daq::startDaq() {
    runDaqCtrl = TASK_START;
    t0 = new thread(&daq::runDaq, this);
    return 1;
}

int daq::stopDaq() {
    runDaqCtrl = TASK_STOP;
    //t0->join();
    t0->detach();
    return 1;
}

void daq::runDaq() {
    daqStatus = TASK_RUN;

    daqSize = 0;
    unsigned int readSize, restSize;
    unsigned long recSize = 0;
    void* netPtr = NULL;
    unsigned long dCount = 0;
    while(1) {
        if((msgrcv(netMsgQue, &daqMsg, sizeof(daqMsg), 0, 0)) < 0) {
            debugMsg << name << "# " << "could not get msg quene: " << daqMsg.size;
            stMsg->stateOut(debugMsg);
            daqStatus = TASK_ERROR;
            break;
        }
        //debugMsg << name << "# " << "fetch netMsg " << daqMsg.size << daqMsg.signal;
        //stMsg->stateOut(debugMsg);

        if(daqMsg.signal == 2) {
            if(!dataPool->netSetSnap()) {
                debugMsg << name << "# " << " could not set snap status.";
                stMsg->stateOut(debugMsg);
                daqStatus = TASK_ERROR;
                break;
            }
            
            //daqSize = dataPool->netGetSnapSize();
            daqSize = daqMsg.size;
            readSize = daqSize;

            //debugMsg << name << "# " << "fetch netMsg " << daqMsg.size << ", daqSize " << daqSize;
            //stMsg->stateOut(debugMsg);

            netPtr = dataPool->netGetSnapPtr(0, readSize);
            if(netPtr != NULL) {
                if(readSize == daqSize) {
                    outFile.write((const char*)netPtr, daqSize);
                }
                else {
                    outFile.write((const char*)netPtr, readSize);
                    restSize = daqSize - readSize;
                    netPtr = dataPool->netGetSnapPtr(readSize, restSize);
                    outFile.write((const char*)netPtr, restSize);
                }
                recSize += daqSize;

                dCount++;
                if(dCount%100==0) {
                    debugMsg << name << "# " << "save " << recSize << " bytes data";
                    stMsg->stateOut(debugMsg);
                }
            }
            else {
                daqStatus = TASK_ERROR;
                dataPool->netPopSnap(daqSize);
                debugMsg << name << "# " << "could not get snap PTR of netRing: " << readSize;
                stMsg->stateOut(debugMsg);
                break;
            }

            dataPool->netPopSnap(daqSize);
            continue;
        }

        if(daqMsg.signal == 3) {
            break;
        }
    }

    if(daqStatus == TASK_RUN)
        daqStatus = TASK_EXIT;

    //debugMsg << name << "# " << "stop thread " << daqStatus;
    //stMsg->stateOut(debugMsg);
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
