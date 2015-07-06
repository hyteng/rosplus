#ifndef dataPacker_h
#define dataPacker_h 

#include <thread>
#include "../src/smBase.h"

class dataPacker :public smBase {

    public:
        dataPacker(const std::string& n);
        ~dataPacker();

    protected:
        virtual int ConfiguredPREP(int argc=0, void* argv[]=NULL);
        virtual int ReadySATR(int argc=0, void* argv[]=NULL);
        virtual int RunningSPTR(int argc=0, void* argv[]=NULL);
        virtual int RunningPAUS(int argc=0, void* argv[]=NULL);
        virtual int PausedSPTR(int argc=0, void* argv[]=NULL);
        virtual int PausedRESU(int argc=0, void* argv[]=NULL);
        virtual int OTFCONF(int argc=0, void* argv[]=NULL);

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
