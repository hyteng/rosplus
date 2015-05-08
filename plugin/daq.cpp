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


daq::daq(const string& n): smBase(n) {
}

daq::~daq() {
}

int daq::LoadedCONF(int para) {
    if(!cfgInfo->infoGetString("config.daq.fileName", fileName))
        return -1;
    return 3;
}

int daq::ConfiguredPREP(int para) {
    stMsg->stateOut(1, "daq ConfiguredPREP");
    beforeDaq();
    return 4;
}

int daq::ReadySATR(int para) {
    stMsg->stateOut(1, "daq ReadySATR");
    startDaq();
    return 5;
}

int daq::RunningSPTR(int para) {
    stMsg->stateOut(1, "daq RunningSPTR");
    stopDaq();
    return 4;
}

int daq::ReadySTOP(int para) {
    stMsg->stateOut(1, "daq ReadySPTR");
    afterDaq();
    return 3;
}

int daq::PausedSPTR(int para) {
    stMsg->stateOut(1, "daq PausedSPTR");
    stopDaq();
    return 4;
}

int daq::beforeDaq() {
    netMsgQue = dataPool->getNetMsgQue();
    outFile.open(fileName, std::ios::out|std::ios::binary|std::ios::trunc);

    return 1;
}

int daq::afterDaq() {
    outFile.close();
    return 1;
}

int daq::startDaq() {
    stMsg->stateOut(1, "start Daq.");
    runDaqCtrl = TASK_START;
    t0 = new thread(&daq::runDaq, this);
    return 1;
}

int daq::stopDaq() {
    stMsg->stateOut(1, "stop Daq.");
    runDaqCtrl = TASK_STOP;
    //t0->join();
    return 1;
}

void daq::runDaq() {

    stMsg->stateOut(1, "running Daq.");

    daqStatus = TASK_RUN;

    daqCount = 0;
    totalDaqSize = 0;
    unsigned int recSize = 0;
    void* netPtr = NULL;
    while(1) {
        if((msgrcv(netMsgQue, &daqMsg, sizeof(daqMsg), 0, 0)) < 0) {
            daqStatus = TASK_ERROR;
            break;
        }
        
        //debugMsg << "daq fetch netMsg " << daqMsg.size << daqMsg.signal;
        //stMsg->stateOut(debugMsg);

        daqCount++;
        totalDaqSize += daqMsg.size;
        if(daqCount >= 5 || daqMsg.signal == 2) {
            dataPool->netSetSnap();
            netPtr = dataPool->netGetSnapPtr(0, totalDaqSize);
            if(netPtr != NULL) {
                // 2 threads
                //ts = new thread(&daq::sendData, this, netPtr, totalDaqSize);
                outFile.write((const char*)netPtr, totalDaqSize);
                //ts->join();

                recSize += totalDaqSize;
                debugMsg << "daq save " << recSize << "data" << endl;
            }
            else {
                daqStatus = TASK_ERROR;
                break;
            }
            dataPool->netPopSnap(totalDaqSize);

            daqCount = 0;
            totalDaqSize = 0;
        }

        if(daqMsg.signal == 2)
            break;
    }
    //if(runDaqCtrl == TASK_STOP || daqStatus == TASK_ERROR) {
        if(daqStatus == TASK_RUN)
            daqStatus = TASK_EXIT;
    //}

    debugMsg << "daq stop thread" << daqStatus;
    stMsg->stateOut(debugMsg);
}

int daq::sendData(void* p0, const unsigned int& nBytes) {
    int sndSize = stMsg->sendData(p0, nBytes);
    return sndSize;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
