#ifndef dataPacker_h
#define dataPacker_h 

#include <thread>
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
        int packData(unsigned int& packSize);
        int packDataTest(unsigned int& packSize);

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
