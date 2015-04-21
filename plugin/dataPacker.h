#ifndef dataPacker_h
#define dataPacker_h 

#include <thread>
#include "../src/smBase.h"

class dataPacker :public smBase {

    public:
        dataPacker(const std::string& n);
        ~dataPacker();

    protected:
        virtual int ConfiguredPREP(int para);
        virtual int ReadySATR(int para);
        virtual int RunningSPTR(int para);
        virtual int PausedSPTR(int para);

    private:
        int startPacker();
        int stopPacker();
        void runPack();
        int packData(unsigned int& packSize);

    private:
        int runPackCtrl;
        int packStatus;
        int devMsgQue, netMsgQue;
        unsigned int packCount;
        unsigned int totalPackSize;
        streamMsg packMsg;
        std::thread *t0;
};
#endif
