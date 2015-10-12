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

int daq::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    configDaq();
    return 3;
}

int daq::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    prepDaq();
    return 4;
}

int daq::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    finishDaq();
    return 3;
}

int daq::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    startDaq();
    return 5;
}

int daq::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    stopDaq();
    return 4;
}

int daq::RunningPAUS(void* argv[]) {
    return 6;
}

int daq::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    stopDaq();
    return 4;
}

int daq::PausedRESU(void* argv[]) {
    return 5;
}

int daq::OTFCONF(void* argv[]) {
    return stId;
}

int daq::configDaq() {
    fileName = "data_sample.txt";
    if(!cfgInfo->infoGetString("config."+name+".fileName", fileName)) {
        debugMsg << name << "# " << "config: could not get config."+name+".fileName";
        stMsg->stateOut(debugMsg);
    }
    return 1;
}

int daq::prepDaq() {
    netMsgQue = dataPool->getNetMsgQue();
    outFile.open(fileName, std::ios::out|std::ios::binary|std::ios::trunc);
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
    return 1;
}

void daq::runDaq() {
    daqStatus = TASK_RUN;

    //daqCount = 0;
    totalDaqSize = 0;
    unsigned int readSize, restSize;
    unsigned int recSize = 0;
    void* netPtr = NULL;
    while(1) {
        if((msgrcv(netMsgQue, &daqMsg, sizeof(daqMsg), 0, 0)) < 0) {
            daqStatus = TASK_ERROR;
            break;
        }
        
        //debugMsg << name << "# " << "fetch netMsg " << daqMsg.size << daqMsg.signal;
        //stMsg->stateOut(debugMsg);

        //daqCount++;
        //totalDaqSize += daqMsg.size;
        if(daqMsg.signal == 2) {
            dataPool->netSetSnap();
            totalDaqSize = dataPool->netGetSnapSize();
            readSize = totalDaqSize;
            netPtr = dataPool->netGetSnapPtr(0, readSize);
            if(netPtr != NULL) {
                if(readSize == totalDaqSize) {
                    outFile.write((const char*)netPtr, totalDaqSize);
                }
                else {
                    outFile.write((const char*)netPtr, readSize);
                    restSize = totalDaqSize - readSize;
                    netPtr = dataPool->netGetSnapPtr(readSize, restSize);
                    outFile.write((const char*)netPtr, restSize);
                }
                recSize += totalDaqSize;
                debugMsg << name << "# " << "save " << recSize << "data";
                stMsg->stateOut(debugMsg);
            }
            else {
                daqStatus = TASK_ERROR;
                break;
            }
            dataPool->netPopSnap(totalDaqSize);

            //daqCount = 0;
            //totalDaqSize = 0;

            continue;
        }

        if(daqMsg.signal == 3) {
            break;
        }
    }
    //if(runDaqCtrl == TASK_STOP || daqStatus == TASK_ERROR) {
        if(daqStatus == TASK_RUN)
            daqStatus = TASK_EXIT;
    //}

    debugMsg << name << "# " << "stop thread" << daqStatus;
    stMsg->stateOut(debugMsg);
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
