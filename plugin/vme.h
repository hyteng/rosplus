#ifndef vme_h
#define vme_h

#include <thread>
#include <stdint.h>
#include "../src/smBase.h"
#include "vmelib.h"

uint32_t imgCtrlAddr[18] = {0x100, 0x114, 0x128, 0x13C, 0x1A0, 0x1B4, 0x1C8, 0x1DC, 0x400, 0x200, 0xF00, 0xF14, 0xF28, 0xF3C, 0xF90, 0xFA4, 0xFB8, 0xFCC};

class vme :public smBase {
    public:
        vme(const std::string& n);
        ~vme();

        virtual bool queryInterface(const std::string& funcName, void* para[], void* ret);
        VMEBridge* getVME() {return pvme;};
        int getImgCtrl(int i, uint32_t& addr);

    protected:
        virtual int InitializedLOAD(void* argv[]=NULL);
        virtual int LoadedUNLD(void* argv[]=NULL);
        virtual int LoadedCONF(void* argv[]=NULL);
        virtual int ConfiguredUNCF(void* argv[]=NULL);
        virtual int ConfiguredPREP(void* argv[]=NULL);
        virtual int ReadySTOP(void* argv[]=NULL);
        virtual int ReadySATR(void* argv[]=NULL);
        virtual int RunningSPTR(void* argv[]=NULL);
        virtual int RunningPAUS(void* argv[]=NULL);
        virtual int PausedSPTR(void* argv[]=NULL);
        virtual int PausedRESU(void* argv[]=NULL);
        virtual int OTFCONF(void* argv[]=NULL);

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
        smBase *pDev;
        unsigned int dmaNumber;
        uint32_t dmaSize, adc0Base;
        uintptr_t dmaBase;
};
#endif
