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
        virtual int LoadedCONF(void* argv[]=NULL);
        virtual int ConfiguredPREP(void* argv[]=NULL);
        virtual int ReadySTOP(void* argv[]=NULL);
        virtual int ReadySATR(void* argv[]=NULL);
        virtual int RunningSPTR(void* argv[]=NULL);
        virtual int RunningPAUS(void* argv[]=NULL);
        virtual int PausedSPTR(void* argv[]=NULL);
        virtual int PausedRESU(void* argv[]=NULL);
        virtual int OTFCONF(void* argv[]=NULL);

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
