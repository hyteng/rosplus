#ifndef dataPacker_h
#define dataPacker_h 

#include "../src/smBase.h"

class dataPacker :public smBase {

    public:
        dataPacker(const std::string& n);
        ~dataPacker();

    protected:
        virtual int ConfiguredPREP(void* argv[]=NULL);
        virtual int ReadySATR(void* argv[]=NULL);
        virtual int RunningSPTR(void* argv[]=NULL);
        virtual int RunningPAUS(void* argv[]=NULL);
        virtual int PausedSPTR(void* argv[]=NULL);
        virtual int PausedRESU(void* argv[]=NULL);
        virtual int OTFCONF(void* argv[]=NULL);

    private:
        int prepPacker();
        int startPacker();
        int stopPacker();
        void runPack();
        int packStringSplit(const std::string& pList);
        int packData(unsigned int& packSize);
        int packDataTest1(unsigned int& packSize); // test is just for adc1785 single board
        int packDataTest2(unsigned int& packSize);
        int fillEvent(unsigned int& packSize);

    private:
        int runPackCtrl;
        int packStatus;
        int devMsgQue, netMsgQue;
        unsigned int packCount;
        unsigned int totalPackSize;
        streamMsg packMsg;
        std::thread *t0;

        smBase *vmeDev;
        int vmeSize, listSize, eventTh;
        std::vector<std::string> vmeList;
        std::vector<std::string> devList;
        std::vector<smBase*> packList;
        std::vector<unsigned int> dataSize;
};
#endif
