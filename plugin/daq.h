#ifndef daq_h
#define daq_h

#include <thread>
#include <fstream>
#include "../src/smBase.h"


class daq :public smBase {

    public:
        daq(const std::string& n);
        ~daq();
        
    protected:
        virtual int LoadedCONF(int argc=0, void* argv[]=NULL);
        virtual int ConfiguredPREP(int argc=0, void* argv[]=NULL);
        virtual int ReadySTOP(int argc=0, void* argv[]=NULL);
        virtual int ReadySATR(int argc=0, void* argv[]=NULL);
        virtual int RunningSPTR(int argc=0, void* argv[]=NULL);
        virtual int RunningPAUS(int argc=0, void* argv[]=NULL);
        virtual int PausedSPTR(int argc=0, void* argv[]=NULL);
        virtual int PausedRESU(int argc=0, void* argv[]=NULL);
        virtual int OTFCONF(int argc=0, void* argv[]=NULL);

    private:
        int configDaq();
        int prepDaq();
        int finishDaq();
        int startDaq();
        int stopDaq();
        void runDaq();
        int sendData(void* p0, const unsigned int& nBytes);

    private:
        int runDaqCtrl;
        int daqStatus;
        int netMsgQue;
        unsigned int daqCount;
        unsigned int totalDaqSize;
        streamMsg daqMsg;
        std::thread *t0, *ts;

        std::string fileName;
        std::fstream outFile;
};

#endif
