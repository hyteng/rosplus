#ifndef vme_h
#define vme_h

#include <stdint.h>
#include "../src/smBase.h"
#include "vmelib.h"

class vme :public smBase {
    public:
        typedef VMEBridge::vmeAddr vmeAddr;
        vme(const std::string& n);
        ~vme();
        virtual int queryInterface(const std::string& funcName, void* para[], void* ret);

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
        
        VMEBridge* getVME() {return pvme;};
        int getImgCtrl(int i, uint32_t& addr);
        std::vector<std::string>& getNameList();
        unsigned int devStringSplit(const string& dList);

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
        unsigned int listNumber, listSize, eventTh;
        std::vector<std::string> nameList;
        std::vector<smBase*> devList;
        std::vector<vmeAddr> buffList;
        std::vector<uint32_t> sizeList; // since the vme has a limit of 256 words on blt
        std::vector<uint32_t> awList;
        std::vector<uint32_t> dwList;
        std::vector<uintptr_t> offsetList;

        
        unsigned int dmaNumber;
        unsigned int dmaSize, tranSize;
        uintptr_t dmaBase;
};
#endif
