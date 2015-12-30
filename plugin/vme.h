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
        virtual int InitializedLOAD(std::string& ret, void* para[]=NULL);
        virtual int LoadedUNLD(std::string& ret, void* para[]=NULL);
        virtual int LoadedCONF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredUNCF(std::string& ret, void* para[]=NULL);
        virtual int ConfiguredPREP(std::string& ret, void* para[]=NULL);
        virtual int ReadySTOP(std::string& ret, void* para[]=NULL);
        virtual int ReadySATR(std::string& ret, void* para[]=NULL);
        virtual int RunningSPTR(std::string& ret, void* para[]=NULL);
        virtual int PausedSPTR(std::string& ret, void* para[]=NULL);

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
