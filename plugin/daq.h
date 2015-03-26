#ifndef daq_h
#define daq_h

#include <string>
#include <thread>
#include "ringBuffer.h"
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
        ringBuffer *ringDma, *ringNet;
        unsigned int sizeDma, sizeNet, dmaTranSize;
        unsigned int totalDmaSize, totalNetSize;
        std::thread *t0;
        int msgQDma, msgQNet;
        int runDmaCtrl, runNetCtrl;
        int dmaStatus, netStatus;

        VMEBridge *pvme;
};

#endif
