#include "daq.h"
#include <thread>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

using std::string;
using std::thread;


daq::daq(const string& n): smBase(n) {
    ring0 = new ringBuffer();
    ring1 = new ringBuffer();
}

daq::~daq() {
    delete ring0;
    delete ring1;
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
    t0 = new thread(&daq::startDaq, this);
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

    if(ring0->isVaild())
        ring0->release();
    if(ring1->isVaild())
        ring1->release();
    ring0->create(size0);
    ring1->create(size1);

    key_t key_dma, key_net;
    if((key_dma=ftok(".", 'vme_dma'))==-1) {
        return 0;
    }
    if((g_msgQcblt=msgget(key_dma, 0)) >= 0) {
        if((msgctl(g_msgQcblt, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((g_msgQcblt=msgget(key_dma, IPC_CREAT|IPC_EXCL|0666))==-1) {
        return 0;
    }

    if((key_net=ftok(".", 'vme_net'))==-1) {
        return 0;
    }
    if((g_msgQnet=msgget(key_net, 0)) >= 0) {
        if((msgctl(g_msgQcblt, IPC_RMID, NULL)) < 0){
            return 0;
        }
    }
    if((g_msgQnet=msgget(key_net, IPC_CREAT|IPC_EXCL|0666))==-1) {
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
    return 1;
}

int daq::stopDaq() {

    stMsg->stateOut(1, "stop Daq.");
    return 1;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new daq(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
