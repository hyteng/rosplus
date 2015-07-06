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
        virtual int InitializedLOAD(int argc=0, void* argv[]=NULL);
        virtual int LoadedUNLD(int argc=0, void* argv[]=NULL);
        virtual int LoadedCONF(int argc=0, void* argv[]=NULL);
        virtual int ConfiguredUNCF(int argc=0, void* argv[]=NULL);
        virtual int ConfiguredPREP(int argc=0, void* argv[]=NULL);
        virtual int ReadySTOP(int argc=0, void* argv[]=NULL);
        virtual int ReadySATR(int argc=0, void* argv[]=NULL);
        virtual int RunningSPTR(int argc=0, void* argv[]=NULL);
        virtual int RunningPAUS(int argc=0, void* argv[]=NULL);
        virtual int PausedSPTR(int argc=0, void* argv[]=NULL);
        virtual int PausedRESU(int argc=0, void* argv[]=NULL);
        virtual int OTFCONF(int argc=0, void* argv[]=NULL);

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
