#ifndef daq_h
#define daq_h

#include <string>
#include <thread>
#include "../src/ringBuffer.h"
#include "../src/smBase.h"

class daq :public smBase {

    public:
        daq(const std::string& n);
        ~daq();
        
    protected:
        virtual int InitializedLOAD(int para);
        virtual int LoadedUNLD(int para);
        virtual int LoadedCONF(int para);
        virtual int ConfiguredUNCF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);
        virtual int RunningPAUS(int para);
        virtual int PausedSTOP(int para);
        virtual int PausedRESU(int para);
        virtual int PausedSATR(int para);
        virtual int SelfTrans(int para);
        virtual int AnyIMPO(int para);
        virtual int AnyEXIT(int para);

        int beforeDaq();
        int afterDaq();
        int startDaq();
        int stopDaq();
        void runDma(void* para);
        void runPack(void* para);
        void runNet(void* para);

    private:
        ringBuffer *dmaRing, *netRing;
        unsigned int dmaRingSize, netRingSize, dmaTranSize;
        unsigned int dmaCount, packCount, netCount;
        unsigned int packSize;
        unsigned int totalDmaSize, totalPackSize, totalNetSize;
        std::thread *t0, *t1, *t2;
        int dmaMsgQue, netMsgQue;
        int runDmaCtrl, runPackCtrl, runNetCtrl;
        int dmaStatus, packStatus, netStatus;

        //VMEBridge *pvme;
        dataPacker *pack;
};

#endif
