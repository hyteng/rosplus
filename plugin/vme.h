#ifndef vme_h
#define vme_h

#include <thread>
#include <stdint.h>
#include "../src/smBase.h"
#include "vmelib.h"

class vme :public smBase {
    public:
        vme(const std::string& n);
        ~vme();
        virtual void* getHelp() {return (void*)pvme;};

    protected:
        virtual int InitializedLOAD(int para);
        virtual int LoadedUNLD(int para);
        virtual int LoadedCONF(int para);
        virtual int ConfiguredUNCF(int para);
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int ReadySTOP(int para);
        virtual int PausedSPTR(int para);

    private:
        int configVme();
        int releaseVme();
        int prepVme();
        int finishVme();
        int startVme();
        int stopVme();
        void runVme();

    private:
        int runVmeCtrl;
        int vmeStatus;
        int devMsgQue;
        unsigned int vmeCount;
        unsigned int totalVmeSize;
        streamMsg vmeMsg;
        std::thread *t0;

        VMEBridge *pvme;
        unsigned int dmaNumber;
        uint32_t dmaSize, adc0Base;
        uintptr_t dmaBase;
};
#endif
