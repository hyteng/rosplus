#ifndef vme_h
#define vme_h

#include <stdint.h>
#include "../src/smBase.h"
#include "vmelib.h"

class vme :public smBase {
    public:
        #define VMEADDRPTR uint32_t
        vme(const std::string& n);
        ~vme();
        virtual bool queryInterface(const std::string& funcName, void* para[], void* ret);

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
        VMEBridge* getVME() {return pvme;};
        int getImgCtrl(int i, uint32_t& addr);
        int devStringSplit(const string& dList);

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
        smBase *triDev;
        int listNumber, listSize, eventTh;
        std::vector<uintptr_t> offsetList;
        std::vector<std::string> devList;
        std::vector<VMEADDRPTR> buffList;
        std::vector<uint32_t> sizeList; // since the vme has a limit of 256 words on blt
        std::vector<uint32_t> awList;
        std::vector<uint32_t> dwList;

        
        unsigned int dmaNumber;
        uint32_t dmaSize;
        uintptr_t dmaBase;
};
#endif
