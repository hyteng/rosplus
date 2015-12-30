#ifndef dataPacker_h
#define dataPacker_h 

#include "../src/smBase.h"

class dataPacker :public smBase {

    public:
        dataPacker(const std::string& n);
        ~dataPacker();

    protected:
        virtual int ConfiguredPREP(std::string& ret, void* para[]=NULL);
        virtual int ReadySATR(std::string& ret, void* para[]=NULL);
        virtual int RunningSPTR(std::string& ret, void* para[]=NULL);
        virtual int PausedSPTR(std::string& ret, void* para[]=NULL);

    private:
        int prepPacker();
        int startPacker();
        int stopPacker();
        void runPack();
        int packStringSplit(const std::string& pList);
        int packData(unsigned int& packSize);
        int packDataTest(unsigned int& packSize); // test is just for adc1785 single board
        int fillEvent(unsigned int& packSize);

    private:
        int runPackCtrl;
        int packStatus;
        int devMsgQue, netMsgQue;
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
